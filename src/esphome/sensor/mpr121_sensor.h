#ifndef ESPHOME_SENSOR_MPR121_H
#define ESPHOME_SENSOR_MPR121_H

#include "esphome/defines.h"

#ifdef USE_MPR121

#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

enum {
  MPR121_SENSOR_TYPE_SLIDER = 0x00,
  MPR121_SENSOR_TYPE_ROTARY = 0x01,
};

class MPR121SensorChannel {
 public:
  MPR121SensorChannel(uint8_t channel_num = 0, uint8_t value = 0);

  uint8_t channel = 0;
  uint8_t value = 0;
};

class MPR121Sensor : public sensor::Sensor {
 public:
  MPR121Sensor(const std::string &name);
  void process(uint16_t *data, uint16_t *last_data);
  void set_sensor_type(uint8_t sensor_type);
  void add_sensor_channel(uint8_t channel, uint8_t value);
  static const uint8_t MPR121_SENSOR_TYPE_SLIDER = 0;

 protected:
  std::vector<MPR121SensorChannel *> channels_{};
  uint8_t sensor_type_ = MPR121_SENSOR_TYPE_SLIDER;
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPR121

#endif  // ESPHOME_BINARY_SENSOR_MPR121_H
