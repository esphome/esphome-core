#include "esphome/defines.h"

#ifdef USE_ROTARY_ENCODER_SENSOR

#include "esphome/sensor/rotary_encoder.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.rotary_encoder";

// based on https://github.com/jkDesignDE/MechInputs/blob/master/QEIx4.cpp
static const uint8_t STATE_LUT_MASK = 0x1C; // clears upper counter increment/decrement bits and pin states
static const uint8_t STATE_PIN_A_HIGH = 0x01;
static const uint8_t STATE_PIN_B_HIGH = 0x02;
static const uint8_t STATE_S0 = 0x00;
static const uint8_t STATE_S1 = 0x04;
static const uint8_t STATE_S2 = 0x08;
static const uint8_t STATE_S3 = 0x0C;
static const uint8_t STATE_CCW = 0x00;
static const uint8_t STATE_CW = 0x10;
static const uint8_t STATE_INCREMENT_COUNTER = 0x20;
static const uint8_t STATE_DECREMENT_COUNTER = 0x40;

// State explanation: 8-bit uint
// Bit 0 (0x01) encodes Pin A HIGH/LOW (reset before each read)
// Bit 1 (0x02) encodes Pin B HIGH/LOW (reset before each read)
// Bit 2&3 (0x0C) encodes state S0-S3
// Bit 4 (0x10) encodes clockwise/counter-clockwise rotation

// output: Bit 5 (0x20) encodes counter increment, Bit 6 (0x40) encodes counter decrement.
static uint8_t ICACHE_RAM_ATTR state_lookup_table[32] = {
    // act state S0 in CCW direction
    STATE_CCW | STATE_S0,                           // 0x00: stay here
    STATE_CW  | STATE_S1 | STATE_INCREMENT_COUNTER, // 0x01: goto CW+S1 and increment counter (dir change)
    STATE_CCW | STATE_S0,                           // 0x02: stay here
    STATE_CCW | STATE_S3 | STATE_DECREMENT_COUNTER, // 0x03: goto CCW+S3 and decrement counter
    // act state S1 in CCW direction
    STATE_CCW | STATE_S1,                           // 0x04: stay here
    STATE_CCW | STATE_S1,                           // 0x05: stay here
    STATE_CCW | STATE_S0 | STATE_DECREMENT_COUNTER, // 0x06: goto CCW+S0 and decrement counter
    STATE_CW  | STATE_S2 | STATE_INCREMENT_COUNTER, // 0x07: goto CW+S2 and increment counter (dir change)
    // act state S2 in CCW direction
    STATE_CCW | STATE_S1 | STATE_DECREMENT_COUNTER, // 0x08: goto CCW+S1 and decrement counter
    STATE_CCW | STATE_S2,                           // 0x09: stay here
    STATE_CW  | STATE_S3 | STATE_INCREMENT_COUNTER, // 0x0A: goto CW+S3 and increment counter (dir change)
    STATE_CCW | STATE_S2,                           // 0x0B: stay here
    // act state S3 in CCW direction
    STATE_CW  | STATE_S0 | STATE_INCREMENT_COUNTER, // 0x0C: goto CW+S0 and increment counter (dir change)
    STATE_CCW | STATE_S2 | STATE_DECREMENT_COUNTER, // 0x0D: goto CCW+S2 and decrement counter
    STATE_CCW | STATE_S3,                           // 0x0E: stay here
    STATE_CCW | STATE_S3,                           // 0x0F: stay here

    // act state S0 in CW direction
    STATE_CW  | STATE_S0,                           // 0x10: stay here
    STATE_CW  | STATE_S1 | STATE_INCREMENT_COUNTER, // 0x11: goto CW+S1 and increment counter
    STATE_CW  | STATE_S0,                           // 0x12: stay here
    STATE_CCW | STATE_S3 | STATE_DECREMENT_COUNTER, // 0x13: goto CCW+S3 and decrement counter (dir change)
    // act state S1 in CW direction
    STATE_CW  | STATE_S1,                           // 0x14: stay here
    STATE_CW  | STATE_S1,                           // 0x15: stay here
    STATE_CCW | STATE_S0 | STATE_DECREMENT_COUNTER, // 0x16: goto CCW+S0 and decrement counter (dir change)
    STATE_CW  | STATE_S2 | STATE_INCREMENT_COUNTER, // 0x17: goto CW+S2 and increment counter
    // act state S2 in CW direction
    STATE_CCW | STATE_S1 | STATE_DECREMENT_COUNTER, // 0x18: goto CCW+S1 and decrement counter (dir change)
    STATE_CW  | STATE_S2,                           // 0x19: stay here
    STATE_CW  | STATE_S3 | STATE_INCREMENT_COUNTER, // 0x1A: goto CW+S3 and increment counter
    STATE_CW  | STATE_S2,
    // act state S3 in CW direction
    STATE_CW  | STATE_S0 | STATE_INCREMENT_COUNTER, // 0x1C: goto CW+S0 and increment counter
    STATE_CCW | STATE_S2 | STATE_DECREMENT_COUNTER, // 0x1D: goto CCW+S2 and decrement counter (dir change)
    STATE_CW  | STATE_S3,                           // 0x1E: stay here
    STATE_CW  | STATE_S3                            // 0x1F: stay here
};

void ICACHE_RAM_ATTR RotaryEncoderSensor::process_state_machine_() {
  // Forget upper bits and add pin states
  uint8_t input_state = this->state_ & STATE_LUT_MASK;
  if (this->pin_a_->digital_read())
    input_state |= STATE_PIN_A_HIGH;
  if (this->pin_b_->digital_read())
    input_state |= STATE_PIN_B_HIGH;

  uint8_t new_state = state_lookup_table[input_state];
  if ((new_state & STATE_INCREMENT_COUNTER) != 0) {
    this->counter_ = std::min(this->counter_ + 1, this->max_value_);
  }
  if ((new_state & STATE_DECREMENT_COUNTER) != 0) {
    this->counter_ = std::max(this->counter_ - 1, this->min_value_);
  }

  if (this->pin_i_ != nullptr && this->pin_i_->digital_read()) {
    this->counter_ = 0;
  }

  this->state_ = new_state;
}
RotaryEncoderSensor::RotaryEncoderSensor(const std::string &name, GPIOPin *pin_a, GPIOPin *pin_b)
    : Sensor(name), Component(), pin_a_(pin_a), pin_b_(pin_b) {

}

void RotaryEncoderSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Rotary Encoder '%s'...", this->name_.c_str());
  int interrupt_a = digitalPinToInterrupt(this->pin_a_->get_pin());
  this->pin_a_->setup();
  attachInterrupt(interrupt_a, RotaryEncoderSensor::encoder_isr_, CHANGE);

  int interrupt_b = digitalPinToInterrupt(this->pin_b_->get_pin());
  this->pin_b_->setup();
  attachInterrupt(interrupt_b, RotaryEncoderSensor::encoder_isr_, CHANGE);

  if (this->pin_i_ != nullptr) {
    this->pin_i_->setup();
  }
  global_rotary_encoders_.push_back(this);
}
void RotaryEncoderSensor::dump_config() {
  LOG_SENSOR("", "Rotary Encoder", this);
  LOG_PIN("  Pin A: ", this->pin_a_);
  LOG_PIN("  Pin B: ", this->pin_b_);
  LOG_PIN("  Pin I: ", this->pin_i_);
}
void ICACHE_RAM_ATTR RotaryEncoderSensor::encoder_isr_() {
  for (auto *encoder : global_rotary_encoders_) {
    encoder->process_state_machine_();
  }
}
void RotaryEncoderSensor::loop() {
  int counter = this->counter_;
  if (this->last_read_ != counter) {
    this->last_read_ = counter;
    this->publish_state(counter);
  }
}
std::string RotaryEncoderSensor::unit_of_measurement() {
  return "steps";
}
std::string RotaryEncoderSensor::icon() {
  return "mdi:rotate-right";
}
int8_t RotaryEncoderSensor::accuracy_decimals() {
  return 0;
}
void RotaryEncoderSensor::set_reset_pin(const GPIOInputPin &pin_i) {
  this->pin_i_ = pin_i.copy();
}

std::vector<RotaryEncoderSensor *> global_rotary_encoders_;

float RotaryEncoderSensor::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void RotaryEncoderSensor::set_min_value(int32_t min_value) { this->min_value_ = min_value; }
void RotaryEncoderSensor::set_max_value(int32_t max_value) { this->max_value_ = max_value; }

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_ROTARY_ENCODER_SENSOR
