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

#ifdef USE_PULSE_COUNTER_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.pulse_counter";

PulseCounterSensorComponent::PulseCounterSensorComponent(const std::string &name, uint8_t pin, uint32_t update_interval)
  : PollingSensorComponent(name, update_interval) {
  this->set_pin(pin);

  this->set_pcnt_unit(next_pcnt_unit);
  next_pcnt_unit = pcnt_unit_t(int(next_pcnt_unit) + 1); // NOLINT
}
uint8_t PulseCounterSensorComponent::get_pin() const {
  return this->pin_;
}
void PulseCounterSensorComponent::set_pin(uint8_t pin) {
  this->pin_ = pin;
}
pcnt_unit_t PulseCounterSensorComponent::get_pcnt_unit() const {
  return this->pcnt_unit_;
}
void PulseCounterSensorComponent::set_pcnt_unit(pcnt_unit_t pcnt_unit) {
  assert(pcnt_unit < PCNT_UNIT_MAX);
  this->pcnt_unit_ = pcnt_unit;
}
pcnt_count_mode_t PulseCounterSensorComponent::get_rising_edge_mode() const {
  return this->rising_edge_mode_;
}
pcnt_count_mode_t PulseCounterSensorComponent::get_falling_edge_mode() const {
  return this->falling_edge_mode_;
}
void PulseCounterSensorComponent::set_edge_mode(pcnt_count_mode_t rising_edge_mode, pcnt_count_mode_t falling_edge_mode) {
  assert(rising_edge_mode < PCNT_COUNT_MAX);
  this->rising_edge_mode_ = rising_edge_mode;
  assert(falling_edge_mode < PCNT_COUNT_MAX);
  this->falling_edge_mode_ = falling_edge_mode;
}
uint16_t PulseCounterSensorComponent::get_filter() const {
  return this->filter_;
}
void PulseCounterSensorComponent::set_filter(uint16_t filter) {
  assert(filter < 1024 && "Filter value can't exceed 1023");
  this->filter_ = filter;
}

const char *EDGE_MODE_TO_STRING[] = {"DISABLE", "INCREMENT", "DECREMENT"};

void PulseCounterSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up pulse counter '%s'...", this->get_name().c_str());
  ESP_LOGCONFIG(TAG, "    PCNT Unit Number: %u", this->pcnt_unit_);
  ESP_LOGCONFIG(TAG, "    Pin %u", this->pin_);
  ESP_LOGCONFIG(TAG, "    Rising Edge: %s", EDGE_MODE_TO_STRING[this->rising_edge_mode_]);
  ESP_LOGCONFIG(TAG, "    Falling Edge: %s", EDGE_MODE_TO_STRING[this->falling_edge_mode_]);

  pcnt_config_t pcnt_config = {
      .pulse_gpio_num = this->pin_,
      .ctrl_gpio_num = PCNT_PIN_NOT_USED,
      .lctrl_mode = PCNT_MODE_KEEP,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = this->rising_edge_mode_,
      .neg_mode = this->falling_edge_mode_,
      .counter_h_lim = 0,
      .counter_l_lim = 0,
      .unit = this->pcnt_unit_,
      .channel = PCNT_CHANNEL_0,
  };
  pcnt_unit_config(&pcnt_config);

  ESP_LOGCONFIG(TAG, "    Pull Mode %u", this->pull_mode_);
  gpio_set_pull_mode(static_cast<gpio_num_t>(this->pin_), this->pull_mode_);


  if (this->filter_) {
    pcnt_set_filter_value(this->pcnt_unit_, this->filter_);
    pcnt_filter_enable(this->pcnt_unit_);
  }

  pcnt_counter_pause(this->pcnt_unit_);
  pcnt_counter_clear(this->pcnt_unit_);
  pcnt_counter_resume(this->pcnt_unit_);
}
float PulseCounterSensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
std::string PulseCounterSensorComponent::unit_of_measurement() {
  return "pulses/min";
}
std::string PulseCounterSensorComponent::icon() {
  return "mdi:pulse";
}
gpio_pull_mode_t PulseCounterSensorComponent::get_pull_mode() const {
  return this->pull_mode_;
}
void PulseCounterSensorComponent::set_pull_mode(gpio_pull_mode_t pull_mode) {
  this->pull_mode_ = pull_mode;
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
int8_t PulseCounterSensorComponent::accuracy_decimals() {
  return 2;
}

pcnt_unit_t next_pcnt_unit = PCNT_UNIT_0;

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_PULSE_COUNTER_SENSOR
