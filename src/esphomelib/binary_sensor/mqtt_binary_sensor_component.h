//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
#define ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/helpers.h"
#include "esphomelib/mqtt/mqtt_component.h"

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
   * @param friendly_name The friendly name.
   * @param binary_sensor The binary sensor to connect the callback to, can be nullptr.
   */
  explicit MQTTBinarySensorComponent(const std::string &friendly_name,
                                     BinarySensor *binary_sensor = nullptr);

  /// Set the Home Assistant device class (see esphomelib::binary_sensor::device_class)
  void set_device_class(const std::string &device_class);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Creates a new callback for publishing state changes to MQTT.
  virtual binary_callback_t create_on_new_state_callback();

  /// Set the device class for this binary sensor.
  std::string get_device_class() const;

  /// Send discovery.
  void setup() override;

  /// Get the payload this binary sensor uses for an ON value.
  std::string get_payload_on() const;
  /// Set the custom payload this binary sensor uses for an ON value.
  void set_payload_on(const std::string &payload_on);
  /// Get the payload this binary sensor uses for an OFF value.
  std::string get_payload_off() const;
  /// Set the custom payload this binary sensor uses for an OFF value.
  void set_payload_off(const std::string &payload_off);

 protected:
  std::string component_type() const override;

  BinarySensor *binary_sensor_{nullptr};
  Optional<std::string> device_class_{};
  bool first_run_{true};
  bool last_state_;
  Optional<std::string> payload_on_{};
  Optional<std::string> payload_off_{};
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
