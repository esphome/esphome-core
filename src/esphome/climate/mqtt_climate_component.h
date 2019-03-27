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

  void set_custom_current_temperature_state_topic(const std::string &custom_current_temperature_state_topic);
  void set_custom_mode_state_topic(const std::string &custom_mode_state_topic);
  void set_custom_mode_command_topic(const std::string &custom_mode_command_topic);
  void set_custom_target_temperature_state_topic(const std::string &custom_target_temperature_state_topic);
  void set_custom_target_temperature_command_topic(const std::string &custom_target_temperature_command_topic);

 protected:
  const std::string get_current_temperature_state_topic() const;
  const std::string get_mode_state_topic() const;
  const std::string get_mode_command_topic() const;
  const std::string get_target_temperature_state_topic() const;
  const std::string get_target_temperature_command_topic() const;

  std::string friendly_name() const override;

  bool publish_state_();

  std::string custom_current_temperature_state_topic_;
  std::string custom_mode_state_topic_;
  std::string custom_mode_command_topic_;
  std::string custom_target_temperature_state_topic_;
  std::string custom_target_temperature_command_topic_;

  ClimateDevice *device_;
};

}  // namespace climate

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_CLIMATE

#endif  // ESPHOME_CORE_MQTT_CLIMATE_COMPONENT_H
