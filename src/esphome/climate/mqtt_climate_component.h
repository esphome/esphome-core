#ifndef ESPHOME_CORE_MQTT_CLIMATE_COMPONENT_H
#define ESPHOME_CORE_MQTT_CLIMATE_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MQTT_CLIMATE

#include "esphome/mqtt/mqtt_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace climate {

class ClimateDevice;

class MQTTClimateComponent : public mqtt::MQTTComponent {
 public:
  MQTTClimateComponent(ClimateDevice *device);
  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;
  bool send_initial_state() override;
  bool is_internal() override;
  std::string component_type() const override;
  void setup() override;

  MQTT_COMPONENT_CUSTOM_TOPIC(current_temperature, state)
  MQTT_COMPONENT_CUSTOM_TOPIC(mode, state)
  MQTT_COMPONENT_CUSTOM_TOPIC(mode, command)
  MQTT_COMPONENT_CUSTOM_TOPIC(target_temperature, state)
  MQTT_COMPONENT_CUSTOM_TOPIC(target_temperature, command)
  MQTT_COMPONENT_CUSTOM_TOPIC(target_temperature_low, state)
  MQTT_COMPONENT_CUSTOM_TOPIC(target_temperature_low, command)
  MQTT_COMPONENT_CUSTOM_TOPIC(target_temperature_high, state)
  MQTT_COMPONENT_CUSTOM_TOPIC(target_temperature_high, command)
  MQTT_COMPONENT_CUSTOM_TOPIC(away, state)
  MQTT_COMPONENT_CUSTOM_TOPIC(away, command)

 protected:
  std::string friendly_name() const override;

  bool publish_state_();

  ClimateDevice *device_;
};

}  // namespace climate

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_CLIMATE

#endif  // ESPHOME_CORE_MQTT_CLIMATE_COMPONENT_H
