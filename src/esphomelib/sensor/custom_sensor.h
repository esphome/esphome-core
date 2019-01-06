#ifndef ESPHOMELIB_SENSOR_CUSTOM_SENSOR_H
#define ESPHOMELIB_SENSOR_CUSTOM_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class CustomSensorConstructor : public Component {
 public:
  CustomSensorConstructor(const std::function<std::vector<Sensor *>()> &init);

  Sensor *get_sensor(int i);

  void dump_config() override;

 protected:
  std::vector<Sensor *> sensors_;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_SENSOR

#endif //ESPHOMELIB_SENSOR_CUSTOM_SENSOR_H
