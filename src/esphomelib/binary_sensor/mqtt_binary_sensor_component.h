//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
#define ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/mqtt/mqtt_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

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

  /// Send Home Assistant discovery info
  void send_discovery(JsonBuffer &buffer, JsonObject &obj, mqtt::SendDiscoveryConfig &config) override;

  /// Get the payload this binary sensor uses for an ON value.
  const std::string &get_payload_on() const;
  /// Set the custom payload this binary sensor uses for an ON value.
  void set_payload_on(std::string payload_on);
  /// Get the payload this binary sensor uses for an OFF value.
  const std::string &get_payload_off() const;
  /// Set the custom payload this binary sensor uses for an OFF value.
  void set_payload_off(std::string payload_off);

  void send_initial_state() override;
  void publish_state(bool state);
  bool is_internal() override;

 protected:
  /// Return the friendly name of this binary sensor.
  std::string friendly_name() const override;
  /// "binary_sensor" component type.
  std::string component_type() const override;

  BinarySensor *binary_sensor_;
  std::string payload_on_{"ON"};
  std::string payload_off_{"OFF"};
};

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
