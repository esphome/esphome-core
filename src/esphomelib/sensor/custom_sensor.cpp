#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_SENSOR

#include "esphomelib/sensor/custom_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

CustomSensorConstructor::CustomSensorConstructor(const std::function<std::vector<Sensor *>()> &init) {
  this->sensors_ = init();
}
Sensor *CustomSensorConstructor::get_sensor(int i) {
  return this->sensors_[i];
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_SENSOR
