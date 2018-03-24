//
// Created by Otto Winter on 29.12.17.
//

#ifndef ESPHOMELIB_FAN_MQTT_FAN_COMPONENT_H
#define ESPHOMELIB_FAN_MQTT_FAN_COMPONENT_H

#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/fan/fan_state.h"

namespace esphomelib {

namespace fan {

class MQTTFanComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTFanComponent(const std::string &friendly_name);

  /// Set a custom oscillation command topic. Defaults to "<base>/oscillation/command".
  void set_custom_oscillation_command_topic(const std::string &topic);
  /// Set a custom oscillation state topic. Defaults to "<base>/oscillation/state".
  void set_custom_oscillation_state_topic(const std::string &topic);
  /// Set a custom speed command topic. Defaults to "<base>/speed/command".
  void set_custom_speed_command_topic(const std::string &topic);
  /// Set a custom speed state topic. Defaults to "<base>/speed/state".
  void set_custom_speed_state_topic(const std::string &topic);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup the fan subscriptions and discovery.
  void setup() override;
  /// Send state via MQTT if state next_send_ is true.
  void loop() override;
  /// Send the full current state to MQTT.
  void send_state();
  /// 'fan' component type for discovery.
  std::string component_type() const override;

  const std::string get_oscillation_command_topic() const;
  const std::string get_oscillation_state_topic() const;
  const std::string get_speed_command_topic() const;
  const std::string get_speed_state_topic() const;

  FanState *get_state() const;
  void set_state(FanState *state);

 protected:
  FanState *state_;
  bool next_send_;
};

} // namespace fan

} // namespace esphomelib

#endif //ESPHOMELIB_FAN_MQTT_FAN_COMPONENT_H
