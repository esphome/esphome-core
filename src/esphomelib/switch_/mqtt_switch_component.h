#ifndef ESPHOMELIB_SWITCH_MQTT_SWITCH_COMPONENT_H
#define ESPHOMELIB_SWITCH_MQTT_SWITCH_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_SWITCH

#include "esphomelib/binary_sensor/mqtt_binary_sensor_component.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/switch_/switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

/** MQTT representation of switches
 *
 * Overrides MQTTBinarySensorComponent with a callback that can write values to hardware.
 */
class MQTTSwitchComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTSwitchComponent(switch_::Switch *switch_);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  void dump_config() override;

  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  bool send_initial_state() override;
  bool is_internal() override;

  bool publish_state(bool state);

 protected:
  std::string friendly_name() const override;

  /// "switch" component type.
  std::string component_type() const override;

  Switch *switch_;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH

#endif //ESPHOMELIB_SWITCH_MQTT_SWITCH_COMPONENT_H
