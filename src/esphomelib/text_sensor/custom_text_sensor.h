#ifndef ESPHOMELIB_TEXT_SENSOR_CUSTOM_TEXT_SENSOR_H
#define ESPHOMELIB_TEXT_SENSOR_CUSTOM_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_TEXT_SENSOR

#include "esphomelib/text_sensor/text_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class CustomTextSensorConstructor {
 public:
  CustomTextSensorConstructor(std::function<std::vector<TextSensor *>()> init);

  TextSensor *get_text_sensor(int i);

 protected:
  std::vector<TextSensor *> text_sensors_;
};

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_TEXT_SENSOR

#endif //ESPHOMELIB_TEXT_SENSOR_CUSTOM_TEXT_SENSOR_H
