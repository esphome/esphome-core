#ifndef ESPHOME_CLIMATEDEVICE_MQTT_CLIMATEDEVICE_COMPONENT_H
#define ESPHOME_CLIMATEDEVICE_MQTT_CLIMATEDEVICE_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MQTT_CLIMATEDEVICE

#include "esphome/mqtt/mqtt_component.h"
#include "esphome/climatedevice/climatedevice.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

class MQTTClimateDeviceComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTClimateDeviceComponent(ClimateDevice *device);

  /// Set a custom current temperature state topic. Defaults to "<base>/current_temperature/state".
  void set_custom_current_temperature_state_topic(const std::string &topic);
  /// Set a custom target mode state topic. Defaults to "<base>/mode/state".
  void set_custom_mode_state_topic(const std::string &topic);
  /// Set a custom target temperature command topic. Defaults to "<base>/mode/command".
  void set_custom_mode_command_topic(const std::string &topic);
  /// Set a custom target temperature state topic. Defaults to "<base>/target_temperature/state".
  void set_custom_target_temperature_state_topic(const std::string &topic);
  /// Set a custom target temperature command topic. Defaults to "<base>/target_temperature/command".
  void set_custom_targer_temperature_command_topic(const std::string &topic);

  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup the climate device subscriptions and discovery.
  void setup() override;
  /// Send the full current state to MQTT.
  bool send_initial_state() override;
  bool publish_state();
  /// 'climatedevice' component type for discovery.
  std::string component_type() const override;

  const std::string get_current_temperature_state_topic() const;
  const std::string get_mode_state_topic() const;
  const std::string get_mode_command_topic() const;
  const std::string get_target_temperature_state_topic() const;
  const std::string get_target_temperature_command_topic() const;

  ClimateDevice *get_device() const;

  bool is_internal() override;

 protected:
  std::string friendly_name() const override;

  std::string custom_current_temperature_state_topic_;
  std::string custom_mode_state_topic_;
  std::string custom_mode_command_topic_;
  std::string custom_target_temperature_state_topic_;
  std::string custom_target_temperature_command_topic_;

  ClimateDevice *device_;
};

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_CLIMATEDEVICE

#endif  // ESPHOME_CLIMATEDEVICE_MQTT_CLIMATEDEVICE_COMPONENT_H
