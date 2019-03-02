#include "esphome/defines.h"

#ifdef USE_MQTT_BINARY_SENSOR

#include "esphome/binary_sensor/mqtt_binary_sensor_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.mqtt";

std::string MQTTBinarySensorComponent::component_type() const { return "binary_sensor"; }

void MQTTBinarySensorComponent::setup() {
  this->binary_sensor_->add_on_state_callback([this](bool state) { this->publish_state(state); });
}

void MQTTBinarySensorComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT Binary Sensor '%s':", this->binary_sensor_->get_name().c_str());
  LOG_MQTT_COMPONENT(true, false)
}
MQTTBinarySensorComponent::MQTTBinarySensorComponent(BinarySensor *binary_sensor)
    : MQTTComponent(), binary_sensor_(binary_sensor) {}
std::string MQTTBinarySensorComponent::friendly_name() const { return this->binary_sensor_->get_name(); }

void MQTTBinarySensorComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->binary_sensor_->get_device_class().empty())
    root["device_class"] = this->binary_sensor_->get_device_class();
  if (this->is_status_)
    root["payload_on"] = mqtt::global_mqtt_client->get_availability().payload_available;
  if (this->is_status_)
    root["payload_off"] = mqtt::global_mqtt_client->get_availability().payload_not_available;
  config.command_topic = false;
}
bool MQTTBinarySensorComponent::send_initial_state() {
  if (this->binary_sensor_->has_state()) {
    return this->publish_state(this->binary_sensor_->state);
  } else {
    return true;
  }
}
bool MQTTBinarySensorComponent::is_internal() { return this->binary_sensor_->is_internal(); }
bool MQTTBinarySensorComponent::publish_state(bool state) {
  if (this->is_status_)
    return true;

  const char *state_s = state ? "ON" : "OFF";
  return this->publish(this->get_state_topic_(), state_s);
}
void MQTTBinarySensorComponent::set_is_status(bool status) { this->is_status_ = status; }

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_BINARY_SENSOR
