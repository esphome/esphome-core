#ifndef ESPHOMELIB_HOMEASSISTANT_BINARY_SENSOR_H
#define ESPHOMELIB_HOMEASSISTANT_BINARY_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_HOMEASSISTANT_BINARY_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

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

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_HOMEASSISTANT_BINARY_SENSOR

#endif //ESPHOMELIB_HOMEASSISTANT_BINARY_SENSOR_H
