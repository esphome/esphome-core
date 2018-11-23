#include "esphomelib/defines.h"

#ifdef USE_STATUS_BINARY_SENSOR

#include "esphomelib/binary_sensor/status_binary_sensor.h"
#include "esphomelib/mqtt/mqtt_client_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

std::string StatusBinarySensor::device_class() {
  return "connectivity";
}
StatusBinarySensor::StatusBinarySensor(const std::string &name)
    : BinarySensor(name) {

}
void StatusBinarySensor::loop() {
  bool status = mqtt::global_mqtt_client->is_connected();
  if (this->last_status_ != status) {
    this->publish_state(status);
    this->last_status_ = status;
  }
}
void StatusBinarySensor::setup() {
  this->publish_state(false);
}
float StatusBinarySensor::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_STATUS_BINARY_SENSOR
