//
// Created by Otto Winter on 26.11.17.
//

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
    if (!this->get_device_class().empty())
      root["device_class"] = this->get_device_class();
    if (this->get_payload_on() != "ON")
      root["payload_on"] = this->get_payload_on();
    if (this->get_payload_off() != "OFF")
      root["payload_off"] = this->get_payload_off();
  }, true, false);
}

MQTTBinarySensorComponent::MQTTBinarySensorComponent(const std::string &friendly_name,
                                                     BinarySensor *binary_sensor)
    : MQTTComponent(friendly_name), binary_sensor_(binary_sensor) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->add_on_new_state_callback(this->create_on_new_state_callback());
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
std::string MQTTBinarySensorComponent::get_device_class() const {
  if (this->device_class_.defined) {
    return this->device_class_.value;
  } else {
    assert(this->binary_sensor_ != nullptr);
    return this->binary_sensor_->device_class();
  }
}

void MQTTBinarySensorComponent::set_device_class(const std::string &device_class) {
  assert_construction_state(this);
  this->device_class_ = device_class;
}
std::string MQTTBinarySensorComponent::get_payload_on() const {
  if (this->payload_on_.defined) {
    return this->payload_on_.value;
  } else {
    return "ON";
  }
}
void MQTTBinarySensorComponent::set_payload_on(const std::string &payload_on) {
  this->payload_on_ = payload_on;
}
std::string MQTTBinarySensorComponent::get_payload_off() const {
  if (this->payload_off_.defined) {
    return this->payload_off_.value;
  } else {
    return "OFF";
  }
}
void MQTTBinarySensorComponent::set_payload_off(const std::string &payload_off) {
  this->payload_off_ = payload_off;
}

} // namespace binary_sensor

} // namespace esphomelib