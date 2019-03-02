#ifndef ESPHOME_SENSOR_TEMPLATE_SENSOR_H
#define ESPHOME_SENSOR_TEMPLATE_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_TEMPLATE_SENSOR

#include "esphome/sensor/sensor.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class TemplateSensor : public PollingSensorComponent {
 public:
  TemplateSensor(const std::string &name, uint32_t update_interval = 60000);

  void set_template(std::function<optional<float>()> &&f);

  void update() override;

  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  optional<std::function<optional<float>()>> f_;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_SENSOR

#endif  // ESPHOME_SENSOR_TEMPLATE_SENSOR_H
