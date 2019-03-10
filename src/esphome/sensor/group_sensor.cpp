#include "esphome/defines.h"

#ifdef USE_GROUP_SENSOR

#include "esphome/sensor/group_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.group_sensor";

void GroupSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GROUP_SENSOR '%s'...", this->name_.c_str());

}
void GroupSensorComponent::dump_config() {
  LOG_SENSOR("", "GROUP_SENSOR", this);
}

void GroupSensorComponent::loop() {

}

float GroupSensorComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

void GroupSensorComponent::add_sensor(binary_sensor::BinarySensor *sensor, float value) {

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_GROUP_SENSOR
