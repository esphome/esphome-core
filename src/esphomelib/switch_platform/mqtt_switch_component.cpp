//
// Created by Otto Winter on 02.12.17.
//

#include "mqtt_switch_component.h"

#include <utility>

namespace esphomelib {

namespace switch_platform {

static const char *TAG = "MQTTSwitch";

using esphomelib::binary_sensor::binary_callback_t;
using esphomelib::mqtt::Availability;

MQTTSwitchComponent::MQTTSwitchComponent(std::string friendly_name)
    : MQTTBinarySensorComponent(std::move(friendly_name), "") {

}

void MQTTSwitchComponent::setup() {
  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {});

  this->subscribe(this->get_command_topic(), [&](const std::string &payload) {
    if (strcasecmp(payload.c_str(), "ON") == 0) {
      ESP_LOGD(TAG, "Turning Switch on.");
      this->write_value_callback_(true);
    } else if (strcasecmp(payload.c_str(), "OFF") == 0) {
      ESP_LOGD(TAG, "Turning Switch off.");
      this->write_value_callback_(false);
    }
  });
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

} // namespace switch_platform

} // namespace esphomelib
