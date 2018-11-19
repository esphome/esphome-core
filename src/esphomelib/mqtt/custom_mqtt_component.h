#ifndef ESPHOMELIB_MQTT_CUSTOM_MQTT_COMPONENT_H
#define ESPHOMELIB_MQTT_CUSTOM_MQTT_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_MQTT_COMPONENT

#include "esphomelib/component.h"
#include "esphomelib/mqtt/mqtt_client_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace mqtt {

class CustomMQTTComponent : public PollingComponent {
 public:
  template<typename T>
  void subscribe(const std::string &topic, uint8_t qos, T &&method);

  template<typename T>
  void subscribe(const std::string &topic, T &&method);

  template<typename T>
  void subscribe_json(const std::string &topic, uint8_t qos, T &&method);

  template<typename T>
  void subscribe_json(const std::string &topic, T &&method);

  void publish(const std::string &topic, const std::string &payload, uint8_t qos, bool retain);

  void publish(const std::string &topic, const std::string &payload);

  void publish(const std::string &topic, float value, int8_t number_decimals = 3);

  void publish(const std::string &topic, int value);

  void publish_json(const std::string &topic, const json_build_t &f, uint8_t qos, bool retain);

  void publish_json(const std::string &topic, const json_build_t &f);

  bool is_connected();
};

template<typename T>
void CustomMQTTComponent::subscribe(const std::string &topic, uint8_t qos, T &&method) {
  auto f = std::bind(method, this, std::placeholders::_1, std::placeholders::_2);
  global_mqtt_client->subscribe(topic, f, qos);
}
template<typename T>
void CustomMQTTComponent::subscribe(const std::string &topic, T &&method) {
  this->subscribe(topic, 0, method);
}
template<typename T>
void CustomMQTTComponent::subscribe_json(const std::string &topic, uint8_t qos, T &&method) {
  auto f = std::bind(method, this, std::placeholders::_1, std::placeholders::_2);
  global_mqtt_client->subscribe_json(topic, f, qos);
}
template<typename T>
void CustomMQTTComponent::subscribe_json(const std::string &topic, T &&method) {
  this->subscribe_json(topic, 0, method);
}

} // namespace mqtt

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_MQTT_COMPONENT

#endif //ESPHOMELIB_MQTT_CUSTOM_MQTT_COMPONENT_H
