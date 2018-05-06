//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/power_supply_component.h"

#include "esphomelib/esphal.h"
#include "esphomelib/log.h"

#ifdef USE_OUTPUT

namespace esphomelib {

static const char *TAG = "power_supply";

void PowerSupplyComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Power Supply...");

  this->pin_->setup();
  this->pin_->digital_write(false);
  this->enabled_ = false;
}

float PowerSupplyComponent::get_setup_priority() const {
  return setup_priority::HARDWARE + 1.0f; // shortly before other hardware
}

PowerSupplyComponent::PowerSupplyComponent(GPIOPin *pin, uint32_t enable_time, uint32_t keep_on_time)
    : pin_(pin), enable_time_(enable_time), keep_on_time_(keep_on_time) {}

bool PowerSupplyComponent::is_enabled() const {
  return this->enabled_;
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

void PowerSupplyComponent::request_high_power() {
  this->cancel_timeout("power-supply-off");
  this->pin_->digital_write(true);

  if (this->active_requests_ == 0) {
    // we need to enable the power supply.
    // cancel old timeout if it exists because we now definitely have a high power mode.
    ESP_LOGI(TAG, "Enabling power supply.");
    delay(this->enable_time_);
  }
  this->enabled_ = true;
  // increase active requests
  this->active_requests_++;

}

void PowerSupplyComponent::unrequest_high_power() {
  this->active_requests_--;
  assert(this->active_requests_ >= 0 && "Some component unrequested high power mode twice!");

  if (this->active_requests_ < 0) {
    // if asserts are disabled we're just going to use 0 as our now counter.
    this->active_requests_ = 0;
  }

  if (this->active_requests_ == 0) {
    // set timeout for power supply off
    this->set_timeout("power-supply-off", this->keep_on_time_, [this](){
      ESP_LOGI(TAG, "Disabling power supply.");
      this->pin_->digital_write(false);
      this->enabled_ = false;
    });
  }
}

} // namespace esphomelib

#endif //USE_OUTPUT
