#ifndef ESPHOME_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
#define ESPHOME_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MQTT_BINARY_SENSOR

#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/mqtt/mqtt_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

/** Simple MQTT front-end component for a binary_sensor.
 *
 * After construction of this class, it should be connected to the BinarySensor by setting the callback returned
 * by create_on_new_state_callback() in BinarySensor::on_new_state().
 */
class MQTTBinarySensorComponent : public mqtt::MQTTComponent {
 public:
  /** Construct a MQTTBinarySensorComponent.
   *
   * @param binary_sensor The binary sensor.
   */
  explicit MQTTBinarySensorComponent(BinarySensor *binary_sensor);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Send discovery.
  void setup() override;

  void dump_config() override;

  /// Send Home Assistant discovery info
  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  void set_is_status(bool status);

  bool send_initial_state() override;
  bool publish_state(bool state);
  bool is_internal() override;

 protected:
  /// Return the friendly name of this binary sensor.
  std::string friendly_name() const override;
  /// "binary_sensor" component type.
  std::string component_type() const override;

  BinarySensor *binary_sensor_;
  bool is_status_{false};
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_BINARY_SENSOR

#endif  // ESPHOME_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
