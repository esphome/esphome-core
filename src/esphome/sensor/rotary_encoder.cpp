#include "esphome/defines.h"

#ifdef USE_ROTARY_ENCODER_SENSOR

#include "esphome/sensor/rotary_encoder.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.rotary_encoder";

// copied from https://github.com/jkDesignDE/MechInputs/blob/master/QEIx4.cpp
static const uint16_t QE_IX4_MASK = 0x1C;
static const uint16_t QE_IX4_1X_INC = 0x0100;
static const uint16_t QE_IX4_2X_INC = 0x0200;
static const uint16_t QE_IX4_4X_INC = 0x0400;
static const uint16_t QE_IX4_1X_DEC = 0x1000;
static const uint16_t QE_IX4_2X_DEC = 0x2000;
static const uint16_t QE_IX4_4X_DEC = 0x4000;
static const uint16_t QE_IX4_DIR = 0x20;
static const uint16_t QE_IX4_A = 1;
static const uint16_t QE_IX4_B = 2;
static const uint16_t QE_IX4_AB = 3;
static const uint16_t QE_IX4_S0 = 0x0;
static const uint16_t QE_IX4_S1 = 0x4;
static const uint16_t QE_IX4_S2 = 0x8;
static const uint16_t QE_IX4_S3 = 0xC;
static const uint16_t QE_IX4_CCW = 0;
static const uint16_t QE_IX4_CW = 0x10;
static const uint16_t QE_IX4_IS_CHG = 0x7700;
static const uint16_t QE_IX4_IS_INC = 0x0700;
static const uint16_t QE_IX4_IS_DEC = 0x7000;

static uint16_t state_lookup_table[32] = {
    // act state S0 in CCW direction
    QE_IX4_CCW | QE_IX4_S0, QE_IX4_CW | QE_IX4_S1 | QE_IX4_A | QE_IX4_4X_INC | QE_IX4_DIR, QE_IX4_CCW | QE_IX4_S0 | QE_IX4_B,
    QE_IX4_CCW | QE_IX4_S3 | QE_IX4_AB | QE_IX4_1X_DEC,
    // act state S1 in CCW direction
    QE_IX4_CCW | QE_IX4_S1, QE_IX4_CCW | QE_IX4_S1 | QE_IX4_A, QE_IX4_CCW | QE_IX4_S0 | QE_IX4_B | QE_IX4_4X_DEC,
    QE_IX4_CW | QE_IX4_S2 | QE_IX4_AB | QE_IX4_1X_INC | QE_IX4_DIR,
    // act state S2 in CCW direction
    QE_IX4_CCW | QE_IX4_S1 | QE_IX4_2X_DEC, QE_IX4_CCW | QE_IX4_S2 | QE_IX4_A,
    QE_IX4_CW | QE_IX4_S3 | QE_IX4_B | QE_IX4_4X_INC | QE_IX4_DIR, QE_IX4_CCW | QE_IX4_S2 | QE_IX4_AB,
    // act state S3 in CCW direction
    QE_IX4_CW | QE_IX4_S0 | QE_IX4_2X_INC | QE_IX4_DIR, QE_IX4_CCW | QE_IX4_S2 | QE_IX4_A | QE_IX4_4X_DEC,
    QE_IX4_CCW | QE_IX4_S3 | QE_IX4_B, QE_IX4_CCW | QE_IX4_S3 | QE_IX4_AB,

    // act state S0 in CW direction
    QE_IX4_CW | QE_IX4_S0, QE_IX4_CW | QE_IX4_S1 | QE_IX4_A | QE_IX4_4X_INC, QE_IX4_CW | QE_IX4_S0 | QE_IX4_B,
    QE_IX4_CCW | QE_IX4_S3 | QE_IX4_AB | QE_IX4_1X_DEC | QE_IX4_DIR,
    // act state S1 in CW direction
    QE_IX4_CW | QE_IX4_S1, QE_IX4_CW | QE_IX4_S1 | QE_IX4_A, QE_IX4_CCW | QE_IX4_S0 | QE_IX4_B | QE_IX4_4X_DEC | QE_IX4_DIR,
    QE_IX4_CW | QE_IX4_S2 | QE_IX4_AB | QE_IX4_1X_INC,
    // act state S2 in CW direction
    QE_IX4_CCW | QE_IX4_S1 | QE_IX4_2X_DEC | QE_IX4_DIR, QE_IX4_CW | QE_IX4_S2 | QE_IX4_A,
    QE_IX4_CW | QE_IX4_S3 | QE_IX4_B | QE_IX4_4X_INC, QE_IX4_CW | QE_IX4_S2 | QE_IX4_AB,
    // act state S3 in CW direction
    QE_IX4_CW | QE_IX4_S0 | QE_IX4_2X_INC, QE_IX4_CCW | QE_IX4_S2 | QE_IX4_A | QE_IX4_4X_DEC | QE_IX4_DIR,
    QE_IX4_CW | QE_IX4_S3 | QE_IX4_B, QE_IX4_CW | QE_IX4_S3 | QE_IX4_AB};

RotaryEncoderSensor::RotaryEncoderSensor(const std::string &name, GPIOPin *pin_a, GPIOPin *pin_b)
    : Sensor(name), Component(), pin_a_(pin_a), pin_b_(pin_b) {}
void RotaryEncoderSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Rotary Encoder '%s'...", this->name_.c_str());
  int interrupt_a = digitalPinToInterrupt(this->pin_a_->get_pin());
  this->pin_a_->setup();
  attachInterrupt(interrupt_a, RotaryEncoderSensor::encoder_isr, CHANGE);

  int interrupt_b = digitalPinToInterrupt(this->pin_b_->get_pin());
  this->pin_b_->setup();
  attachInterrupt(interrupt_b, RotaryEncoderSensor::encoder_isr, CHANGE);

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
void ICACHE_RAM_ATTR RotaryEncoderSensor::encoder_isr() {
  for (auto *encoder : global_rotary_encoders_) {
    encoder->process_state_machine_();
  }
}
void ICACHE_RAM_ATTR RotaryEncoderSensor::process_state_machine_() {
  this->state_ &= QE_IX4_MASK;
  if (this->pin_a_->digital_read())
    this->state_ |= QE_IX4_A;
  if (this->pin_b_->digital_read())
    this->state_ |= QE_IX4_B;

  this->state_ = state_lookup_table[this->state_];
  this->state_ &= this->resolution_;

  if ((this->state_ & QE_IX4_IS_CHG) != 0) {
    bool counter_change = false;

    if ((this->state_ & QE_IX4_IS_INC) != 0) {
      this->counter_ = std::min(this->counter_ + 1, this->max_value_);
      counter_change = true;
    }
    if ((this->state_ & QE_IX4_IS_DEC) != 0) {
      this->counter_ = std::max(this->counter_ - 1, this->min_value_);
      counter_change = true;
    }

    if (counter_change && this->pin_i_ != nullptr && this->pin_i_->digital_read()) {
      this->counter_ = 0;
    }

    this->has_changed_ = this->has_changed_ || counter_change;
  }
}
void RotaryEncoderSensor::loop() {
  if (this->has_changed_) {
    this->has_changed_ = false;
    this->publish_state(this->counter_);
  }
}
std::string RotaryEncoderSensor::unit_of_measurement() { return "steps"; }
std::string RotaryEncoderSensor::icon() { return "mdi:rotate-right"; }
int8_t RotaryEncoderSensor::accuracy_decimals() { return 0; }
void RotaryEncoderSensor::set_resolution(RotaryEncoderResolution mode) { this->resolution_ = mode; }
void RotaryEncoderSensor::set_reset_pin(const GPIOInputPin &pin_i) { this->pin_i_ = pin_i.copy(); }

std::vector<RotaryEncoderSensor *> global_rotary_encoders_;

float RotaryEncoderSensor::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
void RotaryEncoderSensor::set_min_value(int32_t min_value) { this->min_value_ = min_value; }
void RotaryEncoderSensor::set_max_value(int32_t max_value) { this->max_value_ = max_value; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_ROTARY_ENCODER_SENSOR
