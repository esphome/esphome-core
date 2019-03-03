#ifndef ESPHOME_HOMEASSISTANT_BINARY_SENSOR_H
#define ESPHOME_HOMEASSISTANT_BINARY_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_HOMEASSISTANT_BINARY_SENSOR

#include "esphome/component.h"
#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

class HomeassistantBinarySensor : public BinarySensor, public Component {
 public:
  HomeassistantBinarySensor(const std::string &name, const std::string &entity_id);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

 protected:
  std::string entity_id_;
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HOMEASSISTANT_BINARY_SENSOR

#endif  // ESPHOME_HOMEASSISTANT_BINARY_SENSOR_H
