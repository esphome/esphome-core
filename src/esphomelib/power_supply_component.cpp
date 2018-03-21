//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/power_supply_component.h"

#include "esphomelib/esphal.h"
#include "esphomelib/log.h"

namespace esphomelib {

static const char *TAG = "power_supply";

void PowerSupplyComponent::setup() {
  ESP_LOGD(TAG, "Setting up Power Supply...");
  ESP_LOGV(TAG, "    Pin: %u", this->pin_);

  this->pin_.setup();
  this->pin_.write_value(false);
  this->enabled_ = false;
}

float PowerSupplyComponent::get_setup_priority() const {
  return setup_priority::HARDWARE + 1.0f;
}

PowerSupplyComponent::PowerSupplyComponent(GPIOOutputPin pin, uint32_t enable_time, uint32_t keep_on_time)
    : pin_(pin), enabled_(false), enable_time_(enable_time), keep_on_time_(keep_on_time) {}

bool PowerSupplyComponent::is_enabled() const {
  return this->enabled_;
}

void PowerSupplyComponent::enable() {
  this->cancel_timeout("power-supply-off");

  this->pin_.write_value(true);

  if (!this->enabled_) {
    ESP_LOGI(TAG, "Enabling power supply.");
    delay(this->enable_time_);
    this->enabled_ = true;
  }

  this->set_timeout("power-supply-off", this->keep_on_time_, [&]() {
    ESP_LOGI(TAG, "Disabling power supply.");
    this->pin_.write_value(false);
    this->enabled_ = false;
  });
}
uint32_t PowerSupplyComponent::get_enable_time() const {
  return this->enable_time_;
}
void PowerSupplyComponent::set_enable_time(uint32_t enable_time) {
  this->enable_time_ = enable_time;
}
uint32_t PowerSupplyComponent::get_keep_on_time() const {
  return this->keep_on_time_;
}
void PowerSupplyComponent::set_keep_on_time(uint32_t keep_on_time) {
  this->keep_on_time_ = keep_on_time;
}
GPIOOutputPin &PowerSupplyComponent::get_pin() {
  return this->pin_;
}
void PowerSupplyComponent::set_pin(const GPIOOutputPin &pin) {
  this->pin_ = pin;
}

} // namespace esphomelib
