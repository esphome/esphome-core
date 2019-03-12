#include "esphome/defines.h"

#ifdef USE_GROUP_SENSOR

#include "esphome/sensor/group_sensor.h"

#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.group_sensor";

BinarySensorChannel::BinarySensorChannel( binary_sensor::BinarySensor *sensor, float value) {
  this->binary_sensor = sensor;
  this->value = value;
 }

bool BinarySensorChannel::get_sensor_state(void) {
  return this->binary_sensor->state;
}

GroupSensorComponent::GroupSensorComponent(const std::string &name): Sensor(name) { }

void GroupSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GROUP_SENSOR '%s'...", this->name_.c_str());
}

void GroupSensorComponent::dump_config() {
  LOG_SENSOR("", "GROUP_SENSOR", this);
}

void GroupSensorComponent::loop() {
  float value = 0.0;
  uint8_t num_sensors = 0;
  for(auto *bs: this->sensors_ ) {
    if(bs->binary_sensor->state) {
      num_sensors++;
      value += bs->value;
    }
  }
 if((last_value_ != value) && num_sensors > 0) {
   ESP_LOGD(TAG, "'%s' : value: %f", this->name_.c_str(), value/num_sensors);
   this->publish_state(value);
   last_value_ = value;
 }
}

float GroupSensorComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

void GroupSensorComponent::add_sensor(binary_sensor::BinarySensor *sensor, float value) { 
  BinarySensorChannel *sensor_channel =  new BinarySensorChannel(sensor,value);
  this->sensors_.push_back(sensor_channel);
}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_GROUP_SENSOR
