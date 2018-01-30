//
// Created by Otto Winter on 29.12.17.
//

#ifndef ESPHOMELIB_FAN_MQTT_FAN_COMPONENT_H
#define ESPHOMELIB_FAN_MQTT_FAN_COMPONENT_H

#include <esphomelib/mqtt/mqtt_component.h>
#include "fan_state.h"

namespace esphomelib {

namespace fan {

class MQTTFanComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTFanComponent(const std::string &friendly_name);

  FanState *get_state() const;
  void set_state(FanState *state);

  std::string component_type() const override;

  void setup() override;

  void set_custom_oscillation_command_topic(const std::string &topic);
  void set_custom_oscillation_state_topic(const std::string &topic);
  void set_custom_speed_command_topic(const std::string &topic);
  void set_custom_speed_state_topic(const std::string &topic);

  void loop() override;

  void send_state();

 protected:
  const std::string get_oscillation_command_topic() const;
  const std::string get_oscillation_state_topic() const;
  const std::string get_speed_command_topic() const;
  const std::string get_speed_state_topic() const;

  FanState *state_;
  bool next_send_;
};

} // namespace fan

} // namespace esphomelib

#endif //ESPHOMELIB_FAN_MQTT_FAN_COMPONENT_H
