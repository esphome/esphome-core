#ifndef ESPHOME_SENSOR_GROUP_SENSOR_COMPONENT_H
#define ESPHOME_SENSOR_GROUP_SENSOR_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_GROUP_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

/** helper class to combine the binary_sensor and its float value to one objects. **/
class BinarySensorChannel {
 public:
  BinarySensorChannel(binary_sensor::BinarySensor *sensor, float value);
  float value;
  binary_sensor::BinarySensor *binary_sensor;
};

/** Class to group binary_sensors to one Sensor.
 *
 * Each binary sensor represents a float value in the group.
 */
class GroupSensorComponent : public Sensor, public Component {
 public:
  GroupSensorComponent(const std::string &name);
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;
  /** Add binary_sensors to the group.
   * Each binary_sensor represents a float value when its state is true
   *
   * @param sensor The binary sensor.
   * @param value  The value this binary_sensor represents
   */
  void add_sensor(binary_sensor::BinarySensor *sensor, float value);

 protected:
  std::vector<BinarySensorChannel *> sensors_{};
  float last_value_ = 0.0;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // GROUP_SENSOR

#endif  // ESPHOME_SENSOR_GROUP_SENSOR_COMPONENT_H
