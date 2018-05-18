//
// Created by Otto Winter on 26.11.17.
//

#include <utility>

#include "esphomelib/binary_sensor/mqtt_binary_sensor_component.h"

#include "esphomelib/log.h"

#ifdef USE_BINARY_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.mqtt";

std::string MQTTBinarySensorComponent::component_type() const {
  return "binary_sensor";
}

void MQTTBinarySensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT binary sensor '%s'...", this->binary_sensor_->get_name().c_str());
  if (!this->binary_sensor_->get_device_class().empty()) {
    ESP_LOGCONFIG(TAG, "    Device Class: '%s'", this->binary_sensor_->get_device_class().c_str());
  }
  this->binary_sensor_->add_on_state_callback([this](bool value) {
    ESP_LOGD(TAG, "'%s': Sending state %s", this->friendly_name().c_str(), value ? "ON" : "OFF");
    std::string state = value ? this->get_payload_on() : this->get_payload_off();
    this->send_message(this->get_state_topic(), state);
  });
}

MQTTBinarySensorComponent::MQTTBinarySensorComponent(BinarySensor *binary_sensor)
    : MQTTComponent(), binary_sensor_(binary_sensor) {
  assert(this->binary_sensor_ != nullptr);
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
std::string MQTTBinarySensorComponent::friendly_name() const {
  return this->binary_sensor_->get_name();
}
void MQTTBinarySensorComponent::send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->binary_sensor_->get_device_class().empty())
    root["device_class"] = this->binary_sensor_->get_device_class();
  if (this->payload_on_ != "ON")
    root["payload_on"] = this->payload_on_;
  if (this->payload_off_ != "OFF")
    root["payload_off"] = this->payload_off_;
  config.command_topic = false;
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR
