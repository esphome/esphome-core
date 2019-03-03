#ifndef ESPHOME_TEXT_SENSOR_TEMPLATE_TEXT_SENSOR_H
#define ESPHOME_TEXT_SENSOR_TEMPLATE_TEXT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_TEMPLATE_TEXT_SENSOR

#include "esphome/text_sensor/text_sensor.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class TemplateTextSensor : public TextSensor, public PollingComponent {
 public:
  TemplateTextSensor(const std::string &name, uint32_t update_interval = 60000);

  void set_template(std::function<optional<std::string>()> &&f);

  void update() override;

  float get_setup_priority() const override;

  void dump_config() override;

 protected:
  optional<std::function<optional<std::string>()>> f_{};
};

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_TEXT_SENSOR

#endif  // ESPHOME_TEXT_SENSOR_TEMPLATE_TEXT_SENSOR_H
