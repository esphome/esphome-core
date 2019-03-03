#ifndef ESPHOME_SENSOR_CUSTOM_SENSOR_H
#define ESPHOME_SENSOR_CUSTOM_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_CUSTOM_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class CustomSensorConstructor : public Component {
 public:
  CustomSensorConstructor(const std::function<std::vector<Sensor *>()> &init);

  Sensor *get_sensor(int i);

  void dump_config() override;

 protected:
  std::vector<Sensor *> sensors_;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_CUSTOM_SENSOR

#endif  // ESPHOME_SENSOR_CUSTOM_SENSOR_H
