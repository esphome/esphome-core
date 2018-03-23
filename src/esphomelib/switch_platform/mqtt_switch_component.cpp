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
    : MQTTBinarySensorComponent(std::move(friendly_name), "", switch_), switch_(switch_) {
  if (switch_ == nullptr)
    return;

  this->switch_->add_on_new_state_callback(this->create_on_new_state_callback());
  this->icon_ = this->switch_->icon();
}

void MQTTSwitchComponent::setup() {
  assert(this->switch_ != nullptr);

  ESP_LOGCONFIG(TAG, "Setting up MQTT switch '%s'", this->friendly_name_.c_str());
  if (!this->icon_.empty())
    ESP_LOGCONFIG(TAG, "    Icon: '%s'", this->icon_.c_str());

  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
    // TODO: Enable this once a new Home Assistant version is out.
    // if (!this->icon_.empty())
    //   root["icon"] = this->icon_;
  });

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
  this->switch_->turn_on();
}
void MQTTSwitchComponent::turn_off() {
  ESP_LOGD(TAG, "Turning Switch off.");
  this->switch_->turn_off();
}
Switch *MQTTSwitchComponent::get_switch() const {
  return this->switch_;
}
void MQTTSwitchComponent::set_switch(Switch *switch_) {
  this->switch_ = switch_;
}
const std::string &MQTTSwitchComponent::get_icon() const {
  return this->icon_;
}
void MQTTSwitchComponent::set_icon(const std::string &icon) {
  this->icon_ = icon;
}

} // namespace switch_platform

} // namespace esphomelib
