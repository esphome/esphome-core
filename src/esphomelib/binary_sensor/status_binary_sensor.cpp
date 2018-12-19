#include "esphomelib/defines.h"

#ifdef USE_STATUS_BINARY_SENSOR

#include "esphomelib/binary_sensor/status_binary_sensor.h"
#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/wifi_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

std::string StatusBinarySensor::device_class() {
  return "connectivity";
}
StatusBinarySensor::StatusBinarySensor(const std::string &name)
    : BinarySensor(name) {

}
void StatusBinarySensor::loop() {
  bool status;
  if (mqtt::global_mqtt_client == nullptr) {
    status = global_wifi_component->is_connected();
  } else {
    status = mqtt::global_mqtt_client->is_connected();
  }

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
bool StatusBinarySensor::is_status_binary_sensor() const {
  return true;
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_STATUS_BINARY_SENSOR
