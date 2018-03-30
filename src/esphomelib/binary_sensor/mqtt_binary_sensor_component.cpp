//
// Created by Otto Winter on 26.11.17.
//

#include <utility>

#include "esphomelib/binary_sensor/mqtt_binary_sensor_component.h"

#include "esphomelib/log.h"

namespace esphomelib {

namespace binary_sensor {

static const char *TAG = "binary_sensor::mqtt";

std::string MQTTBinarySensorComponent::component_type() const {
  return "binary_sensor";
}

void MQTTBinarySensorComponent::setup() {
  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
    if (!this->device_class_.empty())
      root["device_class"] = this->device_class_;
    if (this->payload_on_ != "ON")
      root["payload_on"] = this->payload_on_;
    if (this->payload_off_ != "OFF")
      root["payload_off"] = this->payload_off_;
  }, true, false);
}

MQTTBinarySensorComponent::MQTTBinarySensorComponent(std::string friendly_name,
                                                     BinarySensor *binary_sensor)
    : MQTTComponent(std::move(friendly_name)), binary_sensor_(binary_sensor) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->add_on_new_state_callback(this->create_on_new_state_callback());
  this->device_class_ = std::move(binary_sensor->device_class());
}

binary_sensor::binary_callback_t MQTTBinarySensorComponent::create_on_new_state_callback() {
  return [&](bool value) {
    assert_setup(this);
    if (!this->first_run_ && this->last_state_ == value)
      // return if we're not in first run and our value is the same.
      return;
    this->first_run_ = false;
    this->last_state_ = value;

    std::string state = value ? this->get_payload_on() : this->get_payload_off();
    this->send_message(this->get_state_topic(), state);
  };
}
const std::string &MQTTBinarySensorComponent::get_device_class() const {
  return this->device_class_;
}

void MQTTBinarySensorComponent::set_device_class(std::string device_class) {
  assert_construction_state(this);
  this->device_class_ = std::move(device_class);
}
const std::string &MQTTBinarySensorComponent::get_payload_on() const {
  return this->payload_on_;
}
void MQTTBinarySensorComponent::set_payload_on(std::string payload_on) {
  this->payload_on_ = std::move(payload_on);
}
const std::string &MQTTBinarySensorComponent::get_payload_off() const {
  return this->payload_off_;
}
void MQTTBinarySensorComponent::set_payload_off(std::string payload_off) {
  this->payload_off_ = std::move(payload_off);
}

} // namespace binary_sensor

} // namespace esphomelib