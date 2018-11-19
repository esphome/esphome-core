#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_MQTT_COMPONENT

#include "esphomelib/mqtt/custom_mqtt_component.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace mqtt {

void CustomMQTTComponent::publish(const std::string &topic, const std::string &payload, uint8_t qos, bool retain) {
  global_mqtt_client->publish(topic, payload, qos, retain);
}
void CustomMQTTComponent::publish(const std::string &topic, const std::string &payload) {
  this->publish(topic, payload, 0, false);
}
void CustomMQTTComponent::publish(const std::string &topic, float value, int8_t number_decimals) {
  auto str = value_accuracy_to_string(value, number_decimals);
  this->publish(topic, str);
}
void CustomMQTTComponent::publish(const std::string &topic, int value) {
  char buffer[24];
  sprintf(buffer, "%d", value);
  this->publish(topic, buffer);
}
void CustomMQTTComponent::publish_json(const std::string &topic, const json_build_t &f, uint8_t qos, bool retain) {
  global_mqtt_client->publish_json(topic, f, qos, retain);
}
void CustomMQTTComponent::publish_json(const std::string &topic, const json_build_t &f) {
  this->publish_json(topic, f, 0, false);
}
bool CustomMQTTComponent::is_connected() {
  return global_mqtt_client != nullptr && global_mqtt_client->is_connected();
}

} // namespace mqtt

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_MQTT_COMPONENT
