#ifndef ESPHOME_HOMEASSISTANT_TEXT_SENSOR_H
#define ESPHOME_HOMEASSISTANT_TEXT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_HOMEASSISTANT_TEXT_SENSOR

#include "esphome/component.h"
#include "esphome/text_sensor/text_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class HomeassistantTextSensor : public TextSensor, public Component {
 public:
  HomeassistantTextSensor(const std::string &name, const std::string &entity_id);
  void dump_config() override;
  void setup() override;

 protected:
  std::string entity_id_;
};

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HOMEASSISTANT_TEXT_SENSOR

#endif  // ESPHOME_HOMEASSISTANT_TEXT_SENSOR_H
