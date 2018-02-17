//
// Created by Otto Winter on 26.11.17.
//

#include "mqtt_binary_sensor_component.h"

namespace esphomelib {

namespace binary_sensor {

static const char *TAG = "MQTTBinarySensor";

std::string MQTTBinarySensorComponent::component_type() const {
  return "binary_sensor";
}

void MQTTBinarySensorComponent::setup() {
  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
    assert(!this->device_class_.empty());
    if (!this->get_device_class().empty())
      root["device_class"] = buffer.strdup(this->get_device_class().c_str());
  }, true, false);
}

MQTTBinarySensorComponent::MQTTBinarySensorComponent(std::string friendly_name,
                                                     std::string device_class)
    : MQTTComponent(std::move(friendly_name)), device_class_(std::move(device_class)) {
}

binary_sensor::binary_callback_t MQTTBinarySensorComponent::create_on_new_state_callback() {
  return [&](bool enabled) {
    assert_setup(this);
    std::string state = enabled ? "ON" : "OFF";
    this->send_message(this->get_state_topic(), state);
  };
}
const std::string &MQTTBinarySensorComponent::get_device_class() const {
  return this->device_class_;
}

void MQTTBinarySensorComponent::set_device_class(const std::string &device_class) {
  assert_construction_state(this);
  this->device_class_ = device_class;
}

} // namespace binary_sensor

} // namespace esphomelib