#ifndef ESPHOME_HOMEASSISTANT_SENSOR_H
#define ESPHOME_HOMEASSISTANT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_HOMEASSISTANT_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

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

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HOMEASSISTANT_SENSOR

#endif  // ESPHOME_HOMEASSISTANT_SENSOR_H
