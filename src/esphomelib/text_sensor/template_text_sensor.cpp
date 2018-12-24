#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_TEXT_SENSOR

#include "esphomelib/text_sensor/template_text_sensor.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.template";

TemplateTextSensor::TemplateTextSensor(const std::string &name, uint32_t update_interval)
    : TextSensor(name), PollingComponent(update_interval) {

}
void TemplateTextSensor::update() {
  auto val = this->f_();
  if (val.has_value()) {
    this->publish_state(*val);
  }
}
float TemplateTextSensor::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
void TemplateTextSensor::set_template(std::function<optional<std::string>()> &&f) {
  this->f_ = std::move(f);
}
void TemplateTextSensor::dump_config() {
  LOG_TEXT_SENSOR("", "Template Sensor", this);
}

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_TEXT_SENSOR
