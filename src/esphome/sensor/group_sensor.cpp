#include "esphome/defines.h"

#ifdef USE_GROUP_SENSOR

#include "esphome/sensor/group_sensor.h"

#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.group_sensor";

BinarySensorChannel::BinarySensorChannel(const binary_sensor::BinarySensor *sensor, float value) { }

GroupSensorComponent::GroupSensorComponent(const std::string &name): Sensor(name) { }

void GroupSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GROUP_SENSOR '%s'...", this->name_.c_str());

}
void GroupSensorComponent::dump_config() {
  LOG_SENSOR("", "GROUP_SENSOR", this);
}

void GroupSensorComponent::loop() {
  float value = 0.0;
  bool to_publish = false;
  for(auto *bs: this->sensors_) {
    if(bs->binary_sensor->state) {
      value += bs->value;
      to_publish = true;
    }
  }
  if(to_publish)
    this->publish_state(value);

}

float GroupSensorComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

void GroupSensorComponent::add_sensor(binary_sensor::BinarySensor *sensor, float value) { 
  BinarySensorChannel *sensor_cahnnel =  new BinarySensorChannel(sensor,value);
  this->sensors_.push_back(sensor_cahnnel);
}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_GROUP_SENSOR
