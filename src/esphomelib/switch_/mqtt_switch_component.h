//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_SWITCH_MQTT_SWITCH_COMPONENT_H
#define ESPHOMELIB_SWITCH_MQTT_SWITCH_COMPONENT_H

#include "esphomelib/binary_sensor/mqtt_binary_sensor_component.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/switch_/switch.h"
#include "esphomelib/defines.h"

#ifdef USE_SWITCH

namespace esphomelib {

namespace switch_ {

/** MQTT representation of switches
 *
 * Overrides MQTTBinarySensorComponent with a callback that can write values to hardware.
 */
class MQTTSwitchComponent : public binary_sensor::MQTTBinarySensorComponent {
 public:
  explicit MQTTSwitchComponent(switch_::Switch *switch_);

  /// Set the icon for this switch. "" for no icon.
  void set_icon(const std::string &icon);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;

  void send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

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

  Optional<std::string> icon_{}; ///< The icon shown here. Not set means use default from switch. Empty means no icon.
  Switch *switch_;
};

} // namespace switch_

} // namespace esphomelib

#endif //USE_SWITCH

#endif //ESPHOMELIB_SWITCH_MQTT_SWITCH_COMPONENT_H
