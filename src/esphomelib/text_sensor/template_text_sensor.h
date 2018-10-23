#ifndef ESPHOMELIB_TEXT_SENSOR_TEMPLATE_TEXT_SENSOR_H
#define ESPHOMELIB_TEXT_SENSOR_TEMPLATE_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_TEXT_SENSOR

#include "esphomelib/text_sensor/text_sensor.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class TemplateTextSensor : public TextSensor, public PollingComponent {
 public:
  TemplateTextSensor(const std::string &name, uint32_t update_interval = 15000);

  void set_template(std::function<optional<std::string>()> &&f);

  void update() override;

  float get_setup_priority() const override;

 protected:
  std::function<optional<std::string>()> f_{};
};

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_TEXT_SENSOR

#endif //ESPHOMELIB_TEXT_SENSOR_TEMPLATE_TEXT_SENSOR_H
