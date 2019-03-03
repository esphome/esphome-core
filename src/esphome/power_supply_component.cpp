#include "esphome/defines.h"

#ifdef USE_OUTPUT

#include "esphome/power_supply_component.h"

#include "esphome/esphal.h"
#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

static const char *TAG = "power_supply";

void PowerSupplyComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Power Supply...");

  this->pin_->setup();
  this->pin_->digital_write(false);
  this->enabled_ = false;

  add_shutdown_hook([this](const char *cause) {
    this->active_requests_ = 0;
    this->pin_->digital_write(false);
  });
}
void PowerSupplyComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Power Supply:");
  LOG_PIN("  Pin: ", this->pin_);
  ESP_LOGCONFIG(TAG, "  Time to enable: %u ms", this->enable_time_);
  ESP_LOGCONFIG(TAG, "  Keep on time: %.1f s", this->keep_on_time_ / 1000.0f);
}

float PowerSupplyComponent::get_setup_priority() const { return setup_priority::PRE_HARDWARE; }

PowerSupplyComponent::PowerSupplyComponent(GPIOPin *pin, uint32_t enable_time, uint32_t keep_on_time)
    : pin_(pin), enable_time_(enable_time), keep_on_time_(keep_on_time) {}

bool PowerSupplyComponent::is_enabled() const { return this->enabled_; }
uint32_t PowerSupplyComponent::get_enable_time() const { return this->enable_time_; }
void PowerSupplyComponent::set_enable_time(uint32_t enable_time) { this->enable_time_ = enable_time; }
uint32_t PowerSupplyComponent::get_keep_on_time() const { return this->keep_on_time_; }
void PowerSupplyComponent::set_keep_on_time(uint32_t keep_on_time) { this->keep_on_time_ = keep_on_time; }

void PowerSupplyComponent::request_high_power() {
  this->cancel_timeout("power-supply-off");
  this->pin_->digital_write(true);

  if (this->active_requests_ == 0) {
    // we need to enable the power supply.
    // cancel old timeout if it exists because we now definitely have a high power mode.
    ESP_LOGD(TAG, "Enabling power supply.");
    delay(this->enable_time_);
  }
  this->enabled_ = true;
  // increase active requests
  this->active_requests_++;
}

void PowerSupplyComponent::unrequest_high_power() {
  this->active_requests_--;
  if (this->active_requests_ < 0) {
    // if asserts are disabled we're just going to use 0 as our now counter.
    this->active_requests_ = 0;
  }

  if (this->active_requests_ == 0) {
    // set timeout for power supply off
    this->set_timeout("power-supply-off", this->keep_on_time_, [this]() {
      ESP_LOGD(TAG, "Disabling power supply.");
      this->pin_->digital_write(false);
      this->enabled_ = false;
    });
  }
}

ESPHOME_NAMESPACE_END

#endif  // USE_OUTPUT
