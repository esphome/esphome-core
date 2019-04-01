#ifndef ESPHOME_FAN_MQTT_FAN_COMPONENT_H
#define ESPHOME_FAN_MQTT_FAN_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MQTT_FAN

#include "esphome/mqtt/mqtt_component.h"
#include "esphome/fan/fan_state.h"

ESPHOME_NAMESPACE_BEGIN

namespace fan {

class MQTTFanComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTFanComponent(FanState *state);

  MQTT_COMPONENT_CUSTOM_TOPIC(oscillation, command)
  MQTT_COMPONENT_CUSTOM_TOPIC(oscillation, state)
  MQTT_COMPONENT_CUSTOM_TOPIC(speed, command)
  MQTT_COMPONENT_CUSTOM_TOPIC(speed, state)

  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup the fan subscriptions and discovery.
  void setup() override;
  /// Send the full current state to MQTT.
  bool send_initial_state() override;
  bool publish_state();
  /// 'fan' component type for discovery.
  std::string component_type() const override;

  FanState *get_state() const;

  bool is_internal() override;

 protected:
  std::string friendly_name() const override;

  FanState *state_;
};

}  // namespace fan

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_FAN

#endif  // ESPHOME_FAN_MQTT_FAN_COMPONENT_H
