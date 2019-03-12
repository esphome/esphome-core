#include "esphome/defines.h"

#ifdef USE_GROUP_SENSOR

#include "esphome/sensor/group_sensor.h"

#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.group_sensor";

/** constructor takes the binary_sensor and its float value **/
BinarySensorChannel::BinarySensorChannel(binary_sensor::BinarySensor *sensor, float value) {
  this->binary_sensor = sensor;
  this->value = value;
}

// constructor for GroupSensorComponent
GroupSensorComponent::GroupSensorComponent(const std::string &name) : Sensor(name) {}

void GroupSensorComponent::setup() { ESP_LOGCONFIG(TAG, "Setting up group_sensor '%s'...", this->name_.c_str()); }

void GroupSensorComponent::dump_config() {
  ESP_LOGD(TAG, "GROUP_SENSOR:");
  LOG_SENSOR("  ", "group_sensor", this);
}

/**
 * The loop checks all binary_sesnor states
 * When the binary_sensor reports a true value for its state, then the float value it represents is added to the
 * total_current_value
 *
 * Only when the  total_current_value changed and at least one sensor reports an active state we publish the sensors
 * average value
 * */
void GroupSensorComponent::loop() {
  float total_current_value = 0.0;
  uint8_t num_active_sensors = 0;
  // check all binary_sensors for its state
  for (auto *bs : this->sensors_) {
    // when binary_sensor->state is true we add its value to the total_current_value
    if (bs->binary_sensor->state) {
      num_active_sensors++;
      total_current_value += bs->value;
    }
  }
  // check it at least one binary_sensor is active and the value changed.
  if ((last_value_ != total_current_value) && num_active_sensors > 0) {
    ESP_LOGD(TAG, "%s - value: %f", this->name_.c_str(), total_current_value / num_active_sensors);
    // publish the average value
    this->publish_state(total_current_value / num_active_sensors);
  }
  last_value_ = total_current_value;
}

float GroupSensorComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

/**
 * add_sensor to the goup
 **/
void GroupSensorComponent::add_sensor(binary_sensor::BinarySensor *sensor, float value) {
  BinarySensorChannel *sensor_channel = new BinarySensorChannel(sensor, value);
  this->sensors_.push_back(sensor_channel);
}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_GROUP_SENSOR
