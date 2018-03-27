//
//  ads1115_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 25.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/ads1115_component.h"
#include "esphomelib/application.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "input::ads1115";

void ADS1115Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS1115...");
  App.assert_i2c_initialized();
  ESP_LOGCONFIG(TAG, "    Address: 0x%02x", this->address_);
  this->adc_ = ADS1115(this->address_);
  if (!this->adc_.testConnection()) {
    ESP_LOGE(TAG, "Connection to ADS1115 with address 0x%02x failed.", this->address_);
    return;
  }
  // copied from initialize()
  this->set_multiplexer_(ADS1115_MUX_P0_N1, true);
  this->set_gain_(ADS1115_PGA_2P048, true);
  this->adc_.setMode(!this->continuous_mode_);
  this->adc_.setRate(this->rate_);
  this->adc_.setComparatorMode(ADS1115_COMP_MODE_HYSTERESIS); // NOLINT
  this->adc_.setComparatorPolarity(ADS1115_COMP_POL_ACTIVE_LOW); // NOLINT
  this->adc_.setComparatorLatchEnabled(false);
  this->adc_.setComparatorQueueMode(ADS1115_COMP_QUE_DISABLE);


  for (unsigned int i = 0; i < this->sensors_.size(); i++) {
    auto *sensor = this->sensors_[i];
    ESP_LOGCONFIG(TAG, "  Sensor %u", i);
    ESP_LOGCONFIG(TAG, "    Multiplexer: %u", sensor->get_multiplexer());
    ESP_LOGCONFIG(TAG, "    Gain: %u", sensor->get_gain());
    ESP_LOGCONFIG(TAG, "    Update Interval: %u", sensor->get_update_interval());

    this->set_interval("", sensor->get_update_interval(), [this, sensor]{
      this->request_measurement_(sensor);
    });
  }
}
float ADS1115Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void ADS1115Component::set_gain_(uint8_t gain, bool force) {
  if (this->current_gain_ == gain && !force)
    // already in right gain mode, nothing to do here.
    return;
  this->adc_.setGain(gain);
  this->current_gain_ = gain;
}
void ADS1115Component::set_multiplexer_(uint8_t multiplexer, bool force) {
  if (this->current_multiplexer_ == multiplexer && !force)
    // already in right multiplexer, nothing to do here.
    return;
  this->adc_.setMultiplexer(multiplexer);
  this->current_multiplexer_ = multiplexer;
}
void ADS1115Component::request_measurement_(ADS1115Sensor *sensor) {
  this->set_multiplexer_(sensor->get_multiplexer());
  this->set_gain_(sensor->get_gain());
  float v = this->adc_.getMilliVolts() / 1000.0f;
  ESP_LOGD(TAG, "Got Voltage=%fV", v);
  sensor->push_new_value(v);
}
ADS1115Sensor *ADS1115Component::get_sensor(uint8_t multiplexer, uint8_t gain, uint32_t update_interval) {
  auto s = new ADS1115Sensor(multiplexer, gain, update_interval);
  this->sensors_.push_back(s);
  return s;
}
ADS1115Component::ADS1115Component(uint8_t address) : address_(address) {}
uint8_t ADS1115Component::get_address() const {
  return this->address_;
}
void ADS1115Component::set_address(uint8_t address) {
  this->address_ = address;
}
bool ADS1115Component::is_continuous_mode() const {
  return this->continuous_mode_;
}
void ADS1115Component::set_continuous_mode(bool continuous_mode) {
  this->continuous_mode_ = continuous_mode;
}
uint8_t ADS1115Component::get_rate() const {
  return this->rate_;
}
void ADS1115Component::set_rate(uint8_t rate) {
  this->rate_ = rate;
}
ADS1115 &ADS1115Component::get_adc() {
  return this->adc_;
}

uint8_t ADS1115Sensor::get_multiplexer() const {
  return this->multiplexer_;
}
void ADS1115Sensor::set_multiplexer(uint8_t multiplexer) {
  this->multiplexer_ = multiplexer;
}
uint8_t ADS1115Sensor::get_gain() const {
  return this->gain_;
}
void ADS1115Sensor::set_gain(uint8_t gain) {
  this->gain_ = gain;
}
uint32_t ADS1115Sensor::get_update_interval() const {
  return this->update_interval_;
}
void ADS1115Sensor::set_update_interval(uint32_t update_interval) {
  this->update_interval_ = update_interval;
}
ADS1115Sensor::ADS1115Sensor(uint8_t multiplexer, uint8_t gain, uint32_t update_interval)
    : multiplexer_(multiplexer), gain_(gain), update_interval_(update_interval) {}
std::string ADS1115Sensor::unit_of_measurement() {
  return sensor::UNIT_OF_MEASUREMENT_VOLT;
}
std::string ADS1115Sensor::icon() {
  return sensor::ICON_VOLTAGE;
}
uint32_t ADS1115Sensor::update_interval() {
  return this->update_interval_;
}
int8_t ADS1115Sensor::accuracy_decimals() {
  return 3;
}

} // namespace sensor

} // namespace esphomelib
