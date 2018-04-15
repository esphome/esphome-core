//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
#define ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/helpers.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

namespace esphomelib {

namespace binary_sensor {

/** Simple MQTT component for a binary_sensor.
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

  void send_discovery(JsonBuffer &buffer, JsonObject &obj, mqtt::SendDiscoveryConfig &config) override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Send discovery.
  void setup() override;

  /// Get the payload this binary sensor uses for an ON value.
  const std::string &get_payload_on() const;
  /// Set the custom payload this binary sensor uses for an ON value.
  void set_payload_on(std::string payload_on);
  /// Get the payload this binary sensor uses for an OFF value.
  const std::string &get_payload_off() const;
  /// Set the custom payload this binary sensor uses for an OFF value.
  void set_payload_off(std::string payload_off);

 protected:
  std::string friendly_name() const override;
  std::string component_type() const override;

  BinarySensor *binary_sensor_;
  std::string payload_on_{"ON"};
  std::string payload_off_{"OFF"};
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //USE_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
