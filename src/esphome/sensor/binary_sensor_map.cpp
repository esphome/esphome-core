#include "esphome/defines.h"

#ifdef USE_BINARY_SENSOR_MAP

#include "esphome/sensor/binary_sensor_map.h"

#include "esphome/log.h"
#include <bitset>

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.binary_sensor_map";

BinarySensorMapChannel::BinarySensorMapChannel(binary_sensor::BinarySensor *sensor, float value) {
  this->binary_sensor = sensor;
  this->value = value;
}

BinarySensorMap::BinarySensorMap(const std::string &name) : Sensor(name) {}

void BinarySensorMap::setup() { ESP_LOGCONFIG(TAG, "Setting up binary_sensor_map '%s'...", this->name_.c_str()); }

void BinarySensorMap::dump_config() {
  ESP_LOGD(TAG, "BINARY_SENSOR_MAP:");
  LOG_SENSOR("  ", "binary_sensor_map", this);
}

void BinarySensorMap::loop() {
  switch (this->sensor_type_) {
    case BINARY_SENSOR_MAP_TYPE_GROUP:
      this->process_group_();
      break;
    case BINARY_SENSOR_MAP_TYPE_SLIDER:
      this->process_slider_();
      break;
    case BINARY_SENSOR_MAP_TYPE_WHEEL:
      this->process_wheel_();
      break;
  }
}

void BinarySensorMap::process_group_() {
  float total_current_value = 0.0;
  uint8_t num_active_sensors = 0;
  bool touched = false;
  uint64_t mask = 0x00;
  uint8_t cur_sens = 0;
  // check all binary_sensors for its state. when active add its value to total_current_value.
  // create a bitmask for the binary_sensor status on all channels
  for (auto *bs : this->sensors_) {
    if (bs->binary_sensor->state) {
      touched = true;
      num_active_sensors++;
      total_current_value += bs->value;
      mask |= 1 << cur_sens;
    }
    cur_sens++;
  }
  // check if the sensor map was touched
  if (touched) {
    // did the bit_mask change or is it a new sensor touch
    if ((last_mask_ != mask) || !this->last_touched_) {
      this->last_touched_ = true;
      float publish_value = total_current_value / num_active_sensors;
      ESP_LOGD(TAG, "%s - touched value: %f", this->name_.c_str(), publish_value);
      this->publish_state(publish_value);
      last_mask_ = mask;
    }
  } else {
    // is this a new sensor release
    if (this->last_touched_) {
      this->last_touched_ = false;
      ESP_LOGD(TAG, "%s - release value: nan", this->name_.c_str());
      this->publish_state(NAN);
    }
  }
}

void BinarySensorMap::process_slider_() { ESP_LOGD(TAG, "SLIDER is not implemented yet."); }
void BinarySensorMap::process_wheel_() { ESP_LOGD(TAG, "WHEEL is not implemented yet."); }
float BinarySensorMap::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

void BinarySensorMap::add_sensor(binary_sensor::BinarySensor *sensor, float value) {
  BinarySensorMapChannel *sensor_channel = new BinarySensorMapChannel(sensor, value);
  this->sensors_.push_back(sensor_channel);
}

void BinarySensorMap::set_sensor_type(uint8_t sensor_type) { this->sensor_type_ = sensor_type; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_BINARY_SENSOR_MAP
