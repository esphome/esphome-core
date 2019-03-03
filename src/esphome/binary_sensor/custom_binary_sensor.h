#ifndef ESPHOME_BINARY_SENSOR_CUSTOM_BINARY_SENSOR_H
#define ESPHOME_BINARY_SENSOR_CUSTOM_BINARY_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_CUSTOM_BINARY_SENSOR

#include "esphome/component.h"
#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

class CustomBinarySensorConstructor : public Component {
 public:
  CustomBinarySensorConstructor(const std::function<std::vector<BinarySensor *>()> &init);

  BinarySensor *get_binary_sensor(int i);

  void dump_config() override;

 protected:
  std::vector<BinarySensor *> binary_sensors_;
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_CUSTOM_BINARY_SENSOR

#endif  // ESPHOME_BINARY_SENSOR_CUSTOM_BINARY_SENSOR_H
