#include "esphome/defines.h"

#ifdef USE_TEMPLATE_TEXT_SENSOR

#include "esphome/text_sensor/template_text_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.template";

TemplateTextSensor::TemplateTextSensor(const std::string &name, uint32_t update_interval)
    : TextSensor(name), PollingComponent(update_interval) {}
void TemplateTextSensor::update() {
  if (!this->f_.has_value())
    return;

  auto val = (*this->f_)();
  if (val.has_value()) {
    this->publish_state(*val);
  }
}
float TemplateTextSensor::get_setup_priority() const { return setup_priority::HARDWARE; }
void TemplateTextSensor::set_template(std::function<optional<std::string>()> &&f) { this->f_ = f; }
void TemplateTextSensor::dump_config() { LOG_TEXT_SENSOR("", "Template Sensor", this); }

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_TEXT_SENSOR
