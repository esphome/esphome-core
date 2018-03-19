//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/power_supply_component.h"

#include "esphomelib/log.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

static const char *TAG = "power_supply";

void esphomelib::PowerSupplyComponent::setup() {
  ESP_LOGD(TAG, "Setting up Power Supply...");
  ESP_LOGV(TAG, "    Pin: %u", this->pin_);

  pinMode(this->pin_, OUTPUT);
  digitalWrite(this->pin_, HIGH);
  this->enabled_ = false;
}

float esphomelib::PowerSupplyComponent::get_setup_priority() const {
  return setup_priority::HARDWARE + 1.0f;
}

esphomelib::PowerSupplyComponent::PowerSupplyComponent(uint8_t pin, uint32_t enable_time, uint32_t keep_on_time)
    : pin_(pin), enabled_(false), enable_time_(enable_time), keep_on_time_(keep_on_time) {}

bool esphomelib::PowerSupplyComponent::is_enabled() const {
  return this->enabled_;
}

void esphomelib::PowerSupplyComponent::enable() {
  this->cancel_timeout("power-supply-off");

  digitalWrite(this->pin_, LOW);

  if (!this->enabled_) {
    ESP_LOGI(TAG, "Enabling power supply.");
    delay(this->enable_time_);
    this->enabled_ = true;
  }

  this->set_timeout("power-supply-off", this->keep_on_time_, [&]() {
    ESP_LOGI(TAG, "Disabling power supply.");
    digitalWrite(this->pin_, HIGH);
    this->enabled_ = false;
  });
}
uint8_t PowerSupplyComponent::get_pin() const {
  return this->pin_;
}
void PowerSupplyComponent::set_pin(uint8_t pin) {
  assert_construction_state(this);
  assert_is_pin(pin);
  this->pin_ = pin;
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
bool PowerSupplyComponent::is_inverted() const {
  return this->inverted_;
}
void PowerSupplyComponent::set_inverted(bool inverted) {
  this->inverted_ = inverted;
}

} // namespace esphomelib
