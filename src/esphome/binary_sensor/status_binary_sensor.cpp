#include "esphome/defines.h"

#ifdef USE_STATUS_BINARY_SENSOR

#include "esphome/binary_sensor/status_binary_sensor.h"
#include "esphome/mqtt/mqtt_client_component.h"
#include "esphome/wifi_component.h"
#include "esphome/util.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.status";

std::string StatusBinarySensor::device_class() { return "connectivity"; }
StatusBinarySensor::StatusBinarySensor(const std::string &name) : BinarySensor(name) {}
void StatusBinarySensor::loop() {
  bool status = network_is_connected();
#ifdef USE_MQTT
  if (mqtt::global_mqtt_client != nullptr) {
    status = mqtt::global_mqtt_client->is_connected();
  }
#endif

  if (this->last_status_ != status) {
    this->publish_state(status);
    this->last_status_ = status;
  }
}
void StatusBinarySensor::setup() { this->publish_state(false); }
float StatusBinarySensor::get_setup_priority() const { return setup_priority::HARDWARE; }
bool StatusBinarySensor::is_status_binary_sensor() const { return true; }
void StatusBinarySensor::dump_config() { LOG_BINARY_SENSOR("", "Status Binary Sensor", this); }

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_STATUS_BINARY_SENSOR
