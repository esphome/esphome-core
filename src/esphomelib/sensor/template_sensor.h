#ifndef ESPHOMELIB_SENSOR_TEMPLATE_SENSOR_H
#define ESPHOMELIB_SENSOR_TEMPLATE_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SENSOR

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class TemplateSensor : public PollingSensorComponent {
 public:
  TemplateSensor(const std::string &name, uint32_t update_interval = 15000);

  void set_template(std::function<optional<float>()> &&f);

  void update() override;

  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  std::function<optional<float>()> f_;
};

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_TEMPLATE_SENSOR

#endif  // ESPHOMELIB_SENSOR_TEMPLATE_SENSOR_H
