//
//  pulse_counter.cpp
//  esphomelib
//
//  Created by Otto Winter on 24.02.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/pulse_counter.h"

#include "esphomelib/log.h"
#include "esphomelib/esphal.h"
#include "esphomelib/espmath.h"

#ifdef USE_PULSE_COUNTER_SENSOR

#ifdef ARDUINO_ARCH_ESP8266
  #include "FunctionalInterrupt.h"
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.pulse_counter";

PulseCounterSensorComponent::PulseCounterSensorComponent(const std::string &name,
                                                         GPIOPin *pin,
                                                         uint32_t update_interval)
  : PollingSensorComponent(name, update_interval), pin_(pin) {
#ifdef ARDUINO_ARCH_ESP32
  this->pcnt_unit_ = next_pcnt_unit;
  next_pcnt_unit = pcnt_unit_t(int(next_pcnt_unit) + 1); // NOLINT
#endif
}
void PulseCounterSensorComponent::set_edge_mode(PulseCounterCountMode rising_edge_mode, PulseCounterCountMode falling_edge_mode) {
  this->rising_edge_mode_ = rising_edge_mode;
  this->falling_edge_mode_ = falling_edge_mode;
}

const char *EDGE_MODE_TO_STRING[] = {"DISABLE", "INCREMENT", "DECREMENT"};

#ifdef ARDUINO_ARCH_ESP32
void PulseCounterSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up pulse counter '%s'...", this->get_name().c_str());
  ESP_LOGCONFIG(TAG, "    PCNT Unit Number: %u", this->pcnt_unit_);
  ESP_LOGCONFIG(TAG, "    Pin %u", this->pin_->get_pin());
  ESP_LOGCONFIG(TAG, "    Rising Edge: %s", EDGE_MODE_TO_STRING[this->rising_edge_mode_]);
  ESP_LOGCONFIG(TAG, "    Falling Edge: %s", EDGE_MODE_TO_STRING[this->falling_edge_mode_]);

  pcnt_count_mode_t rising = PCNT_COUNT_DIS, falling = PCNT_COUNT_DIS;
  switch (this->rising_edge_mode_) {
    case PULSE_COUNTER_DISABLE: rising = PCNT_COUNT_DIS; break;
    case PULSE_COUNTER_INCREMENT: rising = PCNT_COUNT_INC; break;
    case PULSE_COUNTER_DECREMENT: rising = PCNT_COUNT_DEC; break;
  }
  switch (this->falling_edge_mode_) {
    case PULSE_COUNTER_DISABLE: falling = PCNT_COUNT_DIS; break;
    case PULSE_COUNTER_INCREMENT: falling = PCNT_COUNT_INC; break;
    case PULSE_COUNTER_DECREMENT: falling = PCNT_COUNT_DEC; break;
  }

  pcnt_config_t pcnt_config = {
      .pulse_gpio_num = this->pin_->get_pin(),
      .ctrl_gpio_num = PCNT_PIN_NOT_USED,
      .lctrl_mode = PCNT_MODE_KEEP,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = rising,
      .neg_mode = falling,
      .counter_h_lim = 0,
      .counter_l_lim = 0,
      .unit = this->pcnt_unit_,
      .channel = PCNT_CHANNEL_0,
  };
  pcnt_unit_config(&pcnt_config);

  if (this->filter_us_ != 0) {
    uint16_t filter_val = std::min(this->filter_us_ * 80u, 1023u);
    ESP_LOGCONFIG(TAG, "    Filter Value: %uus (val=%u)", this->filter_us_, filter_val);
    pcnt_set_filter_value(this->pcnt_unit_, filter_val);
    pcnt_filter_enable(this->pcnt_unit_);
  }

  pcnt_counter_pause(this->pcnt_unit_);
  pcnt_counter_clear(this->pcnt_unit_);
  pcnt_counter_resume(this->pcnt_unit_);
}
void PulseCounterSensorComponent::update() {
  int16_t counter;
  pcnt_get_counter_value(this->pcnt_unit_, &counter);
  int16_t delta = counter - this->last_value_;
  this->last_value_ = counter;
  float value = (60000.0f * delta) / float(this->get_update_interval()); // per minute

  ESP_LOGD(TAG, "'%s': Retrieved counter (raw=%d): %0.2f pulses/min",
           this->get_name().c_str(), counter, value);
  this->push_new_value(value);
}
#endif

#ifdef ARDUINO_ARCH_ESP8266
void PulseCounterSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up pulse counter '%s'...", this->get_name().c_str());
  this->pin_->setup();
  ESP_LOGCONFIG(TAG, "    Rising Edge: %s", EDGE_MODE_TO_STRING[this->rising_edge_mode_]);
  ESP_LOGCONFIG(TAG, "    Falling Edge: %s", EDGE_MODE_TO_STRING[this->falling_edge_mode_]);

  auto intr = std::bind(&PulseCounterSensorComponent::gpio_intr, this);
  attachInterrupt(this->pin_->get_pin(), intr, CHANGE);
}

void PulseCounterSensorComponent::update() {
  const int16_t counter = this->counter_;
  int16_t delta = counter - this->last_value_;
  this->last_value_ = counter;
  float value = (60000.0f * delta) / float(this->get_update_interval()); // per minute

  ESP_LOGD(TAG, "'%s': Retrieved counter (raw=%d): %0.2f pulses/min",
           this->get_name().c_str(), counter, value);
  this->push_new_value(value);
}
void PulseCounterSensorComponent::gpio_intr() {
  const uint32_t now = micros();
  if (now - this->last_pulse_ < this->filter_us_) {
    return;
  }
  PulseCounterCountMode mode = this->pin_->digital_read() ? this->rising_edge_mode_ : this->falling_edge_mode_;
  switch (mode) {
    case PULSE_COUNTER_DISABLE: break;
    case PULSE_COUNTER_INCREMENT:
      this->counter_++;
      break;
    case PULSE_COUNTER_DECREMENT:
      this->counter_--;
      break;
  }
  this->last_pulse_ = now;
}
#endif

float PulseCounterSensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
std::string PulseCounterSensorComponent::unit_of_measurement() {
  return "pulses/min";
}
std::string PulseCounterSensorComponent::icon() {
  return "mdi:pulse";
}
int8_t PulseCounterSensorComponent::accuracy_decimals() {
  return 2;
}
void PulseCounterSensorComponent::set_filter_us(uint32_t filter_us) {
  this->filter_us_ = filter_us;
}

#ifdef ARDUINO_ARCH_ESP32
pcnt_unit_t next_pcnt_unit = PCNT_UNIT_0;
#endif

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_PULSE_COUNTER_SENSOR
