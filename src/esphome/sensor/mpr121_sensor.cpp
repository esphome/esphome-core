#include "esphome/defines.h"
#ifdef USE_MPR121

#include "esphome/sensor/mpr121_sensor.h"
#include "esphome/log.h"
#include <bitset>

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.mpr121";

MPR121SensorChannel::MPR121SensorChannel(uint8_t channel, float value) {
  this->channel = channel;
  this->value = value;
}

MPR121Sensor::MPR121Sensor(const std::string &name) : Sensor(name) {
  this->state = 0.0;
 }

void MPR121Sensor::process(uint16_t *data, uint16_t *last_data) {
  //process depend on the sensor_type_
  switch(this->sensor_type_) {
    case MPR121_SENSOR_TYPE_SLIDER:
      this->process_slider_(*data);
      break;
    case MPR121_SENSOR_TYPE_WHEEL:
      this->process_wheel_(*data, *last_data);
      break;
  }
}
/** Proces the data from the touch sensor for Slider type
 * 
 * In this method we check what channels where touched
 * Then publish the average value for all toched channels
 * 
 * */
void MPR121Sensor::process_slider_(uint16_t data) {
  uint8_t count = 0;
  float value = 0;
  // loop through the channels added to this sensor
  for(auto *chan : this->channels_) {
    // when the bit for a certain channel is set
    if (data & (1 << chan->channel)) {
      // we add the channels value to the total value
      value += chan->value;
      count++;
    }
  }
  // only when at least one channel bit was set we publish the new average value
  if(count > 0) {
    this->publish_state(value/count);
    ESP_LOGD(TAG, "'%s': Sending new state %f", this->name_.c_str(), state);
  }
}

void MPR121Sensor::process_wheel_(uint16_t data, uint16_t last_data) {
  uint16_t bits = 0;
  uint16_t last_bits = 0;
  float value = 0;
  // get the bit value for all channels
  bits |= (data & this->mask_);
  if(bits != 0) {
    last_bits |= (last_data & this->mask_);

    if(getFirstSetBitPos(last_bits) <= getFirstSetBitPos(bits)) {
      this->publish_state(this->get_state() + this->step_size_);
    } else {
      this->publish_state(this->get_state() - this->step_size_ * 2);
    }
  }
}

uint16_t MPR121Sensor::getFirstSetBitPos(uint16_t data) {
  return log2(data & -data) + 1;
}

void MPR121Sensor::set_sensor_type(uint8_t sensor_type) {
  //TODO: no logging in setters
  ESP_LOGD(TAG, "'%s': Set sensor type %d", this->name_.c_str(), sensor_type);
  this->sensor_type_ = sensor_type;
}

void MPR121Sensor::set_step_size(uint8_t step_size) {
  //TODO: no logging in setters
  ESP_LOGD(TAG, "'%s': Set step size %d", this->name_.c_str(), step_size);
  this->step_size_ = step_size;
}

void MPR121Sensor::add_sensor_channel(uint8_t channel, float value) {
  //TODO: no logging in setters
  ESP_LOGD(TAG, "'%s': add_sensor_channel %d value %f", this->name_.c_str(), channel, value);
  this->mask_ |= (1 << channel);
  //TODO: no logging in setters
  ESP_LOGD(TAG, "'%s': bitmask %s", this->name_.c_str(), std::bitset<16>(this->mask_).to_string().c_str());
  this->channels_.push_back(new sensor::MPR121SensorChannel(channel,value));
}


}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPR121
