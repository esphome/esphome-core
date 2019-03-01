#include "esphome/defines.h"
#ifdef USE_MPR121

#include "esphome/sensor/mpr121_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.mpr121";

MPR121SensorChannel::MPR121SensorChannel(uint8_t channel, float value) {
  this->channel = channel;
  this->value = value;
}

MPR121Sensor::MPR121Sensor(const std::string &name) : Sensor(name) { }

void MPR121Sensor::process(uint16_t *data, uint16_t *last_data) {
  //process depend on the sensor_type_
  switch(this->sensor_type_){
    case MPR121_SENSOR_TYPE_SLIDER: 
      this->process_slider_(data);
      break;
    case MPR121_SENSOR_TYPE_ROTARY: 
      ESP_LOGD(TAG, "'%s': Unimplemented sensor type %d", this->name_.c_str(), this->sensor_type_);
      break;
    default:
      ESP_LOGE(TAG, "'%s': Unknown sensor type %d", this->name_.c_str(), this->sensor_type_);
  }
}

void MPR121Sensor::process_slider_(uint16_t *data) {
  uint8_t count = 0;
  float value = 0;
  for(auto *chan : this->channels_) {
    if ((*data & (1 << chan->channel)) ) {
      value += chan->value;
      count++;
    }
  }
  if(count > 0) {
    this->publish_state(value/count);
    ESP_LOGD(TAG, "'%s': Sending new state %f", this->name_.c_str(), state);
  }
}

void MPR121Sensor::set_sensor_type(uint8_t sensor_type) {
  this->sensor_type_ = sensor_type;
}

void MPR121Sensor::add_sensor_channel(uint8_t channel, float value) {
    this->channels_.push_back(new sensor::MPR121SensorChannel(channel,value));
}


}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPR121
