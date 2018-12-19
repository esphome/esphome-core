#ifndef ESPHOMELIB_BINARY_SENSOR_CUSTOM_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_CUSTOM_BINARY_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_BINARY_SENSOR

#include "esphomelib/binary_sensor/binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

class CustomBinarySensorConstructor {
 public:
  CustomBinarySensorConstructor(const std::function<std::vector<BinarySensor *>()> &init);

  BinarySensor *get_binary_sensor(int i);

 protected:
  std::vector<BinarySensor *> binary_sensors_;
};

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_CUSTOM_BINARY_SENSOR_H
