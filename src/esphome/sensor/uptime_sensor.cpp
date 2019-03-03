#include "esphome/defines.h"

#ifdef USE_UPTIME_SENSOR

#include "esphome/sensor/uptime_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.uptime";

UptimeSensor::UptimeSensor(const std::string &name, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval) {}
void UptimeSensor::update() {
  const uint32_t ms = millis();
  const uint64_t ms_mask = (1ULL << 32) - 1ULL;
  const uint32_t last_ms = this->uptime_ & ms_mask;
  if (ms < last_ms) {
    this->uptime_ += ms_mask + 1ULL;
    ESP_LOGD(TAG, "Detected roll-over \xf0\x9f\xa6\x84");
  }
  this->uptime_ &= ~ms_mask;
  this->uptime_ |= ms;

  // Do separate second and milliseconds conversion to avoid floating point division errors
  // Probably some IEEE standard already guarantees this division can be done without loss
  // of precision in a single division, but let's do it like this to be sure.
  const uint64_t seconds_int = this->uptime_ / 1000ULL;
  const float seconds = float(seconds_int) + (this->uptime_ % 1000ULL) / 1000.0f;
  this->publish_state(seconds);
}
std::string UptimeSensor::unit_of_measurement() { return "s"; }
std::string UptimeSensor::icon() { return "mdi:timer"; }
int8_t UptimeSensor::accuracy_decimals() { return 0; }
std::string UptimeSensor::unique_id() { return get_mac_address() + "-uptime"; }
float UptimeSensor::get_setup_priority() const { return setup_priority::HARDWARE; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_UPTIME_SENSOR
