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

MQTTSwitchComponent::MQTTSwitchComponent(std::string friendly_name)
    : MQTTBinarySensorComponent(std::move(friendly_name), "") {

}

void MQTTSwitchComponent::setup() {
  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {});

  this->subscribe(this->get_command_topic(), [&](const std::string &payload) {
    if (strcasecmp(payload.c_str(), "ON") == 0) {
      this->send_state(true);
    } else if (strcasecmp(payload.c_str(), "OFF") == 0) {
      this->send_state(false);
    }
  });

  this->send_state(global_preferences.get_bool(this->friendly_name_, "state", false));
}

std::string MQTTSwitchComponent::component_type() const {
  return "switch";
}

void MQTTSwitchComponent::set_write_value_callback(const binary_callback_t &write_callback) {
  this->write_value_callback_ = write_callback;
}
const binary_callback_t &MQTTSwitchComponent::get_write_value_callback() const {
  return this->write_value_callback_;
}
void MQTTSwitchComponent::send_state(bool state) {
  if (state)
    ESP_LOGD(TAG, "Turning Switch on.");
  else
    ESP_LOGD(TAG, "Turning Switch off.");
  this->write_value_callback_(state);
}
binary_callback_t MQTTSwitchComponent::create_on_new_state_callback() {
  return [&](bool enabled) {
    assert_setup(this);
    std::string state = enabled ? "ON" : "OFF";
    this->send_message(this->get_state_topic(), state);
    global_preferences.put_bool(this->friendly_name_, "state", enabled);
  };
}

} // namespace switch_platform

} // namespace esphomelib
