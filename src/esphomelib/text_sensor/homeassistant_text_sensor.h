#ifndef ESPHOMELIB_HOMEASSISTANT_TEXT_SENSOR_H
#define ESPHOMELIB_HOMEASSISTANT_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_HOMEASSISTANT_TEXT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/text_sensor/text_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class HomeassistantTextSensor : public TextSensor {
 public:
  HomeassistantTextSensor(const std::string &name, const std::string &entity_id);
 protected:
  std::string entity_id_;
};

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_HOMEASSISTANT_TEXT_SENSOR

#endif //ESPHOMELIB_HOMEASSISTANT_TEXT_SENSOR_H
