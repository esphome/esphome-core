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

/** SensorChannel class just a placeholder for each mpr121 sensor channel.
 *
 * The channels together form one Sensor. 
 * The Sensor value is calculated by the values of the channels that are active.
 */
class MPR121SensorChannel {
 public:
  MPR121SensorChannel(uint8_t channel_num = 0, float value = 0.0);

  uint8_t channel = 0;
  float value = 0.0;
};

/** Sensor class for all sensor-type functions of the mpr121.
 *
 * This class processes the touch action on the channels
 * and publishes the calculated value for this sensor
 * based on the sensortype SIDER or ROTARY
 */
class MPR121Sensor : public sensor::Sensor {
 public:


  MPR121Sensor(const std::string &name);
  void process(uint16_t *data, uint16_t *last_data);
  void set_sensor_type(uint8_t sensor_type);
  void add_sensor_channel(uint8_t channel, float value);

 protected:
  std::vector<MPR121SensorChannel *> channels_{};
  uint8_t sensor_type_ = MPR121_SENSOR_TYPE_SLIDER;
  void process_slider_(uint16_t *data);
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPR121

#endif  // ESPHOME_BINARY_SENSOR_MPR121_H
