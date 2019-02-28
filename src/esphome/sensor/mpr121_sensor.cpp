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
  //process should depend on the sensor_type_
  for(auto *chan : this->channels_) {
    if ((*data & (1 << chan->channel)) && !(*last_data & (1 << chan->channel))) {
      this->publish_state(chan->value);
      ESP_LOGD(TAG,"process channel value %.2f",chan->value);
    }
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
