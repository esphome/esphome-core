#include "esphomelib/defines.h"

#ifdef USE_ESP32_HALL_SENSOR

#include "esphomelib/sensor/esp32_hall_sensor.h"
#include "esphomelib/esphal.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

ESP32HallSensor::ESP32HallSensor(const std::string &name, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval) {
}
void ESP32HallSensor::update() {
  float value = hallRead() / 4095.0f;
  this->publish_state(value * 10000.0f);
}
std::string ESP32HallSensor::unit_of_measurement() {
  return "µT";
}
std::string ESP32HallSensor::icon() {
  return "mdi:magnet";
}
int8_t ESP32HallSensor::accuracy_decimals() {
  return -1;
}
std::string ESP32HallSensor::unique_id() {
  return get_mac_address() + "-hall";
}

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_ESP32_HALL_SENSOR
