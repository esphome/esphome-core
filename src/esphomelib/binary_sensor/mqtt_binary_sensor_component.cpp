#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphomelib/binary_sensor/mqtt_binary_sensor_component.h"
#include "esphomelib/log.h"

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

  auto f = std::bind(&MQTTBinarySensorComponent::publish_state, this, std::placeholders::_1);
  this->binary_sensor_->add_on_state_callback(f);
}

MQTTBinarySensorComponent::MQTTBinarySensorComponent(BinarySensor *binary_sensor)
    : MQTTComponent(), binary_sensor_(binary_sensor) {

}
std::string MQTTBinarySensorComponent::friendly_name() const {
  return this->binary_sensor_->get_name();
}
void MQTTBinarySensorComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->binary_sensor_->get_device_class().empty())
    root["device_class"] = this->binary_sensor_->get_device_class();
  if (this->is_status_)
    root["payload_on"] = mqtt::global_mqtt_client->get_availability().payload_available;
  if (this->is_status_)
    root["payload_off"] = mqtt::global_mqtt_client->get_availability().payload_not_available;
  config.command_topic = false;
}

void MQTTBinarySensorComponent::send_initial_state() {
  if (this->binary_sensor_->has_state())
    this->publish_state(this->binary_sensor_->state);
}
bool MQTTBinarySensorComponent::is_internal() {
  return this->binary_sensor_->is_internal();
}
void MQTTBinarySensorComponent::publish_state(bool state) {
  if (this->is_status_)
    return;

  const char *state_s = state ? "ON" : "OFF";
  ESP_LOGD(TAG, "'%s': Sending state %s", this->friendly_name().c_str(), state_s);
  this->send_message(this->get_state_topic(), state_s);
}
void MQTTBinarySensorComponent::set_is_status(bool status) {
  this->is_status_ = status;
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR
