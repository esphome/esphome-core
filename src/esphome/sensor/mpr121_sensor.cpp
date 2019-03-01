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
      this->process_slider_(data);
      break;
    case MPR121_SENSOR_TYPE_ROTARY:
      this->process_rotary_(data, last_data);
      break;
    default:
      ESP_LOGE(TAG, "'%s': Unknown sensor type %d", this->name_.c_str(), this->sensor_type_);
  }
}

void MPR121Sensor::process_slider_(uint16_t *data) {
  uint8_t count = 0;
  float value = 0;
  for(auto *chan : this->channels_) {
    if (*data & (1 << chan->channel)) {
      value += chan->value;
      count++;
    }
  }
  if(count > 0) {
    this->publish_state(value/count);
    ESP_LOGD(TAG, "'%s': Sending new state %f", this->name_.c_str(), state);
  }
}

void MPR121Sensor::process_rotary_(uint16_t *data, uint16_t *last_data) {
  std::bitset<16> bits = 0;
  std::bitset<16> last_bits = 0;
  float value = 0;
  // get the bit value for all channels
  bits |= (*data & this->mask_);
  last_bits |= (*last_data & this->mask_);

  ESP_LOGD(TAG, "'%s': bits %s", this->name_.c_str(), bits.to_string().c_str());
  if(last_bits._Find_first() <= bits._Find_first()) {
    this->publish_state(this->get_state() + this->step_size_);
  } else {
    this->publish_state(this->get_state() - this->step_size_ * 2);
  }
}

void MPR121Sensor::set_sensor_type(uint8_t sensor_type) {
  ESP_LOGD(TAG, "'%s': Set sensor type %d", this->name_.c_str(), sensor_type);
  this->sensor_type_ = sensor_type;
}

void MPR121Sensor::set_step_size(uint8_t step_size) {
  ESP_LOGD(TAG, "'%s': Set step size %d", this->name_.c_str(), step_size);
  this->step_size_ = step_size;
}

void MPR121Sensor::add_sensor_channel(uint8_t channel, float value) {
  ESP_LOGD(TAG, "'%s': add_sensor_channel %d value %f", this->name_.c_str(), channel, value);
  this->mask_ |= (1 << channel);
  ESP_LOGD(TAG, "'%s': bitmask %s", this->name_.c_str(), std::bitset<16>(this->mask_).to_string().c_str());
  this->channels_.push_back(new sensor::MPR121SensorChannel(channel,value));
}


}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPR121
