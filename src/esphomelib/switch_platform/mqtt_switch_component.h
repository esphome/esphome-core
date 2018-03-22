//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_SWITCH_PLATFORM_MQTT_SWITCH_COMPONENT_H
#define ESPHOMELIB_SWITCH_PLATFORM_MQTT_SWITCH_COMPONENT_H

#include "esphomelib/binary_sensor/mqtt_binary_sensor_component.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/switch_platform/switch.h"

namespace esphomelib {

namespace switch_platform {

/** MQTTSwitchComponent - MQTT representation of switches
 *
 * Overrides MQTTBinarySensorComponent with a callback that can write values to hardware.
 */
class MQTTSwitchComponent : public binary_sensor::MQTTBinarySensorComponent {
 public:
  explicit MQTTSwitchComponent(std::string friendly_name, switch_platform::Switch *switch_ = nullptr);

  void set_on_set_state_callback(const binary_sensor::binary_callback_t &set_state_callback);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  
  /// Override MQTTBinarySensor's on state received from hw callback.
  binary_sensor::binary_callback_t create_on_new_state_callback() override;

 protected:
  const binary_sensor::binary_callback_t &get_on_set_state_callback() const;
  
  std::string component_type() const override;

  void turn_on();

  void turn_off();

  binary_sensor::binary_callback_t on_set_state_callback_{nullptr};
};

} // namespace switch_platform

} // namespace esphomelib

#endif //ESPHOMELIB_SWITCH_PLATFORM_MQTT_SWITCH_COMPONENT_H
