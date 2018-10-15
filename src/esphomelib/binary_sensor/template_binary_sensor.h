#ifndef ESPHOMELIB_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_BINARY_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

class TemplateBinarySensor : public Component, public BinarySensor {
 public:
  explicit TemplateBinarySensor(const std::string &name);

  void set_template(std::function<optional<bool>()> &&f);

  void loop() override;

  float get_setup_priority() const override;

 protected:
  std::function<optional<bool>()> f_;
  bool last_state_{false};
  bool is_first_state_{true};
};

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H
