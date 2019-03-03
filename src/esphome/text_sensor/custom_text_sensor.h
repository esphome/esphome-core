#ifndef ESPHOME_TEXT_SENSOR_CUSTOM_TEXT_SENSOR_H
#define ESPHOME_TEXT_SENSOR_CUSTOM_TEXT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_CUSTOM_TEXT_SENSOR

#include "esphome/text_sensor/text_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class CustomTextSensorConstructor : public Component {
 public:
  CustomTextSensorConstructor(std::function<std::vector<TextSensor *>()> init);

  TextSensor *get_text_sensor(int i);

  void dump_config() override;

 protected:
  std::vector<TextSensor *> text_sensors_;
};

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_CUSTOM_TEXT_SENSOR

#endif  // ESPHOME_TEXT_SENSOR_CUSTOM_TEXT_SENSOR_H
