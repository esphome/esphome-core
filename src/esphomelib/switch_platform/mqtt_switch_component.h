//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_SWITCH_PLATFORM_MQTT_SWITCH_COMPONENT_H
#define ESPHOMELIB_SWITCH_PLATFORM_MQTT_SWITCH_COMPONENT_H

#include <esphomelib/binary_sensor/mqtt_binary_sensor_component.h>
#include <esphomelib/mqtt/mqtt_component.h>

namespace esphomelib {

namespace switch_platform {

/// Overrides MQTTBinarySensorComponent with a callback that can write values to hardware.
class MQTTSwitchComponent : public binary_sensor::MQTTBinarySensorComponent {
 public:
  explicit MQTTSwitchComponent(std::string friendly_name);

  void setup() override;

  void set_write_value_callback(const binary_sensor::binary_callback_t &write_callback);
  const binary_sensor::binary_callback_t &get_write_value_callback() const;

 protected:
  std::string component_type() const override;

  void send_state(bool state);

  binary_sensor::binary_callback_t write_value_callback_;
};

} // namespace switch_platform

} // namespace esphomelib

#endif //ESPHOMELIB_SWITCH_PLATFORM_MQTT_SWITCH_COMPONENT_H
