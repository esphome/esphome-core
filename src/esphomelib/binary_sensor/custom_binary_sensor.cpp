#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_BINARY_SENSOR

#include "esphomelib/binary_sensor/custom_binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

CustomBinarySensorConstructor::CustomBinarySensorConstructor(const std::function<std::vector<BinarySensor *>()> &init) {
  this->binary_sensors_ = init();
}
BinarySensor *CustomBinarySensorConstructor::get_binary_sensor(int i) {
  return this->binary_sensors_[i];
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_BINARY_SENSOR
