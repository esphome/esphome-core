//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/switch_platform/mqtt_switch_component.h"

#include <utility>

#include "esphomelib/log.h"
#include "esphomelib/esppreferences.h"

namespace esphomelib {

namespace switch_platform {

static const char *TAG = "switch::mqtt";

using esphomelib::binary_sensor::binary_callback_t;
using esphomelib::mqtt::Availability;

MQTTSwitchComponent::MQTTSwitchComponent(std::string friendly_name, switch_platform::Switch *switch_)
    : MQTTBinarySensorComponent(std::move(friendly_name), "", switch_) {
  if (switch_ == nullptr)
    return;

  this->on_set_state_callback_ = switch_->create_on_set_state_callback();
  switch_->set_on_new_state_callback(this->create_on_new_state_callback());
}

void MQTTSwitchComponent::setup() {
  assert(this->on_set_state_callback_ != nullptr);

  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {});

  this->subscribe(this->get_command_topic(), [&](const std::string &payload) {
    if (strcasecmp(payload.c_str(), "ON") == 0)
      this->turn_on();
    else if (strcasecmp(payload.c_str(), "OFF") == 0)
      this->turn_off();
  });

  bool initial_state = global_preferences.get_bool(this->friendly_name_, "state", false);
  if (initial_state) this->turn_on(); else this->turn_off();
}

std::string MQTTSwitchComponent::component_type() const {
  return "switch";
}
binary_callback_t MQTTSwitchComponent::create_on_new_state_callback() {
  return [&](bool enabled) {
    assert_setup(this);
    std::string state = enabled ? "ON" : "OFF";
    this->send_message(this->get_state_topic(), state);
    global_preferences.put_bool(this->friendly_name_, "state", enabled);
  };
}
void MQTTSwitchComponent::turn_on() {
  ESP_LOGD(TAG, "Turning Switch on.");
  this->on_set_state_callback_(true);
}
void MQTTSwitchComponent::turn_off() {
  ESP_LOGD(TAG, "Turning Switch off.");
  this->on_set_state_callback_(false);
}
void MQTTSwitchComponent::set_on_set_state_callback(const binary_sensor::binary_callback_t &set_state_callback) {
  this->on_set_state_callback_ = set_state_callback;
}
const binary_sensor::binary_callback_t &MQTTSwitchComponent::get_on_set_state_callback() const {
  return this->on_set_state_callback_;
}

} // namespace switch_platform

} // namespace esphomelib
