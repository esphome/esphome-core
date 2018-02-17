//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
#define ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H

#include <esphomelib/mqtt/mqtt_component.h>
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/helpers.h"
#include "esphomelib/mqtt/mqtt_component.h"

namespace esphomelib {

namespace binary_sensor {

/// Home Assistant device classes. See <a href="https://home-assistant.io/components/binary_sensor/">Binary Sensor</a>.
namespace device_class {

const std::string COLD = "cold";
const std::string CONNECTIVITY = "connectivity";
const std::string GAS = "gas";
const std::string HEAT = "heat";
const std::string LIGHT = "light";
const std::string MOISTURE = "moisture";
const std::string MOTION = "motion";
const std::string MOVING = "moving";
const std::string OCCUPANCY = "occupancy";
const std::string OPENING = "opening";
const std::string PLUG = "plug";
const std::string POWER = "power";
const std::string SAFETY = "safety";
const std::string SMOKE = "smoke";
const std::string SOUND = "sound";
const std::string VIBRATION = "vibration";

}

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
   * @param device_class The device class of this component (see esphomelib::binary_sensor::device_class)
   */
  explicit MQTTBinarySensorComponent(std::string friendly_name, std::string device_class = "");

  /// Creates a new callback for publishing state changes to MQTT.
  virtual binary_callback_t create_on_new_state_callback();

  /// Set the Home Assistant device class (see esphomelib::binary_sensor::device_class)
  void set_device_class(const std::string &device_class);
  const std::string &get_device_class() const;

  void setup() override;

 protected:
  std::string component_type() const override;

  std::string device_class_;
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //ESPHOMELIB_BINARY_SENSOR_MQTT_BINARY_SENSOR_COMPONENT_H
