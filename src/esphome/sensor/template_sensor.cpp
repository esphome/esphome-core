#include "esphome/defines.h"

#ifdef USE_TEMPLATE_SENSOR

#include "esphome/sensor/template_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.template";

TemplateSensor::TemplateSensor(const std::string &name, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval) {}
void TemplateSensor::update() {
  if (!this->f_.has_value())
    return;

  auto val = (*this->f_)();
  if (val.has_value()) {
    this->publish_state(*val);
  }
}
float TemplateSensor::get_setup_priority() const { return setup_priority::HARDWARE; }
void TemplateSensor::set_template(std::function<optional<float>()> &&f) { this->f_ = f; }
void TemplateSensor::dump_config() {
  LOG_SENSOR("", "Template Sensor", this);
  LOG_UPDATE_INTERVAL(this);
}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_SENSOR
