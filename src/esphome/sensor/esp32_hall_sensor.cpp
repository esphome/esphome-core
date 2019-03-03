#include "esphome/defines.h"

#ifdef USE_ESP32_HALL_SENSOR

#include "esphome/sensor/esp32_hall_sensor.h"
#include "esphome/esphal.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.esp32_hall";

ESP32HallSensor::ESP32HallSensor(const std::string &name, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval) {}
void ESP32HallSensor::update() {
  float value = (hallRead() / 4095.0f) * 10000.0f;
  ESP_LOGD(TAG, "'%s': Got reading %.0f µT", this->name_.c_str(), value);
  this->publish_state(value);
}
std::string ESP32HallSensor::unit_of_measurement() { return "µT"; }
std::string ESP32HallSensor::icon() { return "mdi:magnet"; }
int8_t ESP32HallSensor::accuracy_decimals() { return -1; }
std::string ESP32HallSensor::unique_id() { return get_mac_address() + "-hall"; }
void ESP32HallSensor::dump_config() { LOG_SENSOR("", "ESP32 Hall Sensor", this); }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_ESP32_HALL_SENSOR
