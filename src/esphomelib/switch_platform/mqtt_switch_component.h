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

/** MQTT representation of switches
 *
 * Overrides MQTTBinarySensorComponent with a callback that can write values to hardware.
 */
class MQTTSwitchComponent : public binary_sensor::MQTTBinarySensorComponent {
 public:
  explicit MQTTSwitchComponent(std::string friendly_name, switch_platform::Switch *switch_ = nullptr);

  /// Set the internal switch object used for sending states, does not register the state callback.
  void set_switch(Switch *switch_);

  /// Set the icon for this switch. "" for no icon.
  void set_icon(const std::string &icon);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  
  /// Override MQTTBinarySensor's on state received from hw callback.
  binary_sensor::binary_callback_t create_on_new_state_callback() override;

 protected:
  /// Get the internal switch use for setting state.
  Switch *get_switch() const;

  /// "switch" component type.
  std::string component_type() const override;

  /// Helper method to turn the switch on.
  void turn_on();
  /// Helper method to turn the switch off.
  void turn_off();

  /// Get the icon for this switch.
  std::string get_icon() const;

  Optional<std::string> icon_; ///< The icon shown here. Not set means use default from switch. Empty means no icon.
  Switch *switch_{nullptr};
};

} // namespace switch_platform

} // namespace esphomelib

#endif //ESPHOMELIB_SWITCH_PLATFORM_MQTT_SWITCH_COMPONENT_H
