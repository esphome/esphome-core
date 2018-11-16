#ifndef ESPHOMELIB_CUSTOM_SENSOR_H
#define ESPHOMELIB_CUSTOM_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_SENSOR

#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class CustomSensorConstructor {
 public:
  CustomSensorConstructor(const std::function<std::vector<Sensor *>()> &init);

  Sensor *get_sensor(int i);

 protected:
  std::vector<Sensor *> sensors_;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_SENSOR

#endif //ESPHOMELIB_CUSTOM_SENSOR_H
