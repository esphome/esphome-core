#include "esphome/defines.h"

#ifdef USE_CUSTOM_SENSOR

#include "esphome/sensor/custom_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.custom";

CustomSensorConstructor::CustomSensorConstructor(const std::function<std::vector<Sensor *>()> &init) {
  this->sensors_ = init();
}
Sensor *CustomSensorConstructor::get_sensor(int i) { return this->sensors_[i]; }
void CustomSensorConstructor::dump_config() {
  for (auto *child : this->sensors_) {
    LOG_SENSOR("", "Custom Sensor", child);
  }
}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_CUSTOM_SENSOR
