#ifndef ESPHOME_VERSION_TEXT_SENSOR_H
#define ESPHOME_VERSION_TEXT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_VERSION_TEXT_SENSOR

#include "esphome/component.h"
#include "esphome/text_sensor/text_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class VersionTextSensor : public TextSensor, public Component {
 public:
  explicit VersionTextSensor(const std::string &name);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  std::string icon() override;
  std::string unique_id() override;
};

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_VERSION_TEXT_SENSOR

#endif  // ESPHOME_VERSION_TEXT_SENSOR_H
