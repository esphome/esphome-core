#ifndef ESPHOME_SENSOR_TCS34725_H
#define ESPHOME_SENSOR_TCS34725_H

#include "esphome/defines.h"

#ifdef USE_TCS34725

#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using TCS35725IlluminanceSensor = sensor::EmptyPollingParentSensor<1, ICON_BRIGHTNESS_5, UNIT_LX>;
using TCS35725ColorTemperatureSensor = sensor::EmptyPollingParentSensor<1, ICON_THERMOMETER, UNIT_K>;
using TCS35725ColorChannelSensor = sensor::EmptyPollingParentSensor<1, ICON_LIGHTBULB, UNIT_PERCENT>;

enum TCS34725IntegrationTime {
  TCS34725_INTEGRATION_TIME_2_4MS = 0xFF,
  TCS34725_INTEGRATION_TIME_24MS = 0xF6,
  TCS34725_INTEGRATION_TIME_50MS = 0xEB,
  TCS34725_INTEGRATION_TIME_101MS = 0xD5,
  TCS34725_INTEGRATION_TIME_154MS = 0xC0,
  TCS34725_INTEGRATION_TIME_700MS = 0x00,
};

enum TCS34725Gain {
  TCS34725_GAIN_1X = 0x00,
  TCS34725_GAIN_4X = 0x01,
  TCS34725_GAIN_16X = 0x02,
  TCS34725_GAIN_60X = 0x03,
};

class TCS34725Component : public PollingComponent, public I2CDevice {
 public:
  TCS34725Component(I2CComponent *parent, uint32_t update_interval = 60000);

  void set_integration_time(TCS34725IntegrationTime integration_time);
  void set_gain(TCS34725Gain gain);

  TCS35725ColorChannelSensor *make_clear_sensor(const std::string &name);
  TCS35725ColorChannelSensor *make_red_sensor(const std::string &name);
  TCS35725ColorChannelSensor *make_green_sensor(const std::string &name);
  TCS35725ColorChannelSensor *make_blue_sensor(const std::string &name);
  TCS35725IlluminanceSensor *make_illuminance_sensor(const std::string &name);
  TCS35725ColorTemperatureSensor *make_color_temperature_sensor(const std::string &name);

  void setup() override;
  float get_setup_priority() const override;
  void update() override;
  void dump_config() override;

 protected:
  TCS35725ColorChannelSensor *clear_sensor_{nullptr};
  TCS35725ColorChannelSensor *red_sensor_{nullptr};
  TCS35725ColorChannelSensor *green_sensor_{nullptr};
  TCS35725ColorChannelSensor *blue_sensor_{nullptr};
  TCS35725IlluminanceSensor *illuminance_sensor_{nullptr};
  TCS35725ColorTemperatureSensor *color_temperature_sensor_{nullptr};
  TCS34725IntegrationTime integration_time_{TCS34725_INTEGRATION_TIME_2_4MS};
  TCS34725Gain gain_{TCS34725_GAIN_1X};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TCS34725

#endif  // ESPHOME_SENSOR_TCS34725_H
