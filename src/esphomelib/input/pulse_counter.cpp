//
//  pulse_counter.cpp
//  esphomelib
//
//  Created by Otto Winter on 24.02.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifdef ARDUINO_ARCH_ESP32

#include "pulse_counter.h"
#include <esphomelib/log.h>
#include <esphomelib/hal.h>
#include <cassert>

namespace esphomelib {

namespace input {

static const char *TAG = "pulse_counter";

PulseCounterSensorComponent::PulseCounterSensorComponent(uint8_t pin, uint32_t interval) {
  this->set_pin(pin);
  this->set_check_interval(interval);

  this->set_pcnt_unit(next_pcnt_unit);
  next_pcnt_unit = pcnt_unit_t(int(next_pcnt_unit) + 1); // NOLINT
}
uint8_t PulseCounterSensorComponent::get_pin() const {
  return this->pin_;
}
void PulseCounterSensorComponent::set_pin(uint8_t pin) {
  assert_construction_state(this);
  assert_is_pin(pin);
  this->pin_ = pin;
}
pcnt_unit_t PulseCounterSensorComponent::get_pcnt_unit() const {
  return this->pcnt_unit_;
}
void PulseCounterSensorComponent::set_pcnt_unit(pcnt_unit_t pcnt_unit) {
  assert_construction_state(this);
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
  assert_construction_state(this);
  assert(rising_edge_mode < PCNT_COUNT_MAX);
  this->rising_edge_mode_ = rising_edge_mode;
  assert(falling_edge_mode < PCNT_COUNT_MAX);
  this->falling_edge_mode_ = falling_edge_mode;
}
uint16_t PulseCounterSensorComponent::get_filter() const {
  return this->filter_;
}
void PulseCounterSensorComponent::set_filter(uint16_t filter) {
  assert_construction_state(this);
  assert(filter < 1024 && "Filter value can't exceed 1023");
  this->filter_ = filter;
}
void PulseCounterSensorComponent::setup() {
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

  gpio_set_pull_mode(static_cast<gpio_num_t>(this->pin_), GPIO_FLOATING);

  ESP_LOGD(TAG, "Setting up pulse counter unit %d", this->pcnt_unit_);
  ESP_LOGD(TAG, "    pin=%d", this->pin_);

  if (this->filter_) {
    pcnt_set_filter_value(this->pcnt_unit_, this->filter_);
    pcnt_filter_enable(this->pcnt_unit_);
  }

  pcnt_counter_pause(this->pcnt_unit_);
  pcnt_counter_clear(this->pcnt_unit_);
  pcnt_counter_resume(this->pcnt_unit_);

  this->set_interval("retrieve_value", this->check_interval_, [&]() {
    int16_t counter;
    pcnt_get_counter_value(this->pcnt_unit_, &counter);
    int16_t delta = counter - this->last_value_;
    this->last_value_ = counter;
    float value = (60000.0f * delta) / float(this->check_interval_); // per minute
    value *= this->multiplier_;

    ESP_LOGD(TAG, "Retrieved counter (%d) delta %d -> %0.2f", counter, delta, value);
    this->push_new_value(value, 2);
  });
}
float PulseCounterSensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
uint32_t PulseCounterSensorComponent::get_check_interval() const {
  return this->check_interval_;
}
void PulseCounterSensorComponent::set_check_interval(uint32_t check_interval) {
  assert_construction_state(this);
  assert(check_interval > 0);
  this->check_interval_ = check_interval;
}
std::string PulseCounterSensorComponent::unit_of_measurement() {
  return "pulses/min";
}
uint8_t PulseCounterSensorComponent::get_pin_mode() const {
  return this->pin_mode_;
}
void PulseCounterSensorComponent::set_pin_mode(uint8_t pin_mode) {
  assert_construction_state(this);
  this->pin_mode_ = pin_mode;
}
float PulseCounterSensorComponent::get_multiplier() const {
  return this->multiplier_;
}
void PulseCounterSensorComponent::set_multiplier(float multiplier) {
  assert_construction_state(this);
  this->multiplier_ = multiplier;
}

pcnt_unit_t next_pcnt_unit = PCNT_UNIT_0;

} // namespace input

} // namespace esphomelib

#endif