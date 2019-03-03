#ifndef ESPHOME_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H
#define ESPHOME_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_TEMPLATE_BINARY_SENSOR

#include "esphome/component.h"
#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

class TemplateBinarySensor : public Component, public BinarySensor {
 public:
  explicit TemplateBinarySensor(const std::string &name);

  void set_template(std::function<optional<bool>()> &&f);

  void loop() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  optional<std::function<optional<bool>()>> f_;
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_BINARY_SENSOR

#endif  // ESPHOME_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H
