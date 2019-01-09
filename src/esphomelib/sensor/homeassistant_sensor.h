#ifndef ESPHOMELIB_HOMEASSISTANT_SENSOR_H
#define ESPHOMELIB_HOMEASSISTANT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_HOMEASSISTANT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class HomeassistantSensor : public Sensor, public Component {
 public:
  HomeassistantSensor(const std::string &name, const std::string &entity_id);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
 protected:
  std::string entity_id_;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_HOMEASSISTANT_SENSOR

#endif //ESPHOMELIB_HOMEASSISTANT_SENSOR_H
