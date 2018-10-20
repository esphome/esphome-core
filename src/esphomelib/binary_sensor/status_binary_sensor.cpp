#include "esphomelib/defines.h"

#ifdef USE_STATUS_BINARY_SENSOR

#include "esphomelib/binary_sensor/status_binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

std::string StatusBinarySensor::device_class() {
  return "connectivity";
}
StatusBinarySensor::StatusBinarySensor(const std::string &name)
    : BinarySensor(name) {
  this->publish_state(true);
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_STATUS_BINARY_SENSOR
