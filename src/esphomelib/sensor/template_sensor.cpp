#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SENSOR

#include "esphomelib/sensor/template_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

TemplateSensor::TemplateSensor(const std::string &name, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval) {

}
void TemplateSensor::update() {
  auto val = this->f_();
  if (val.has_value()) {
    this->publish_state(*val);
  }
}
float TemplateSensor::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
void TemplateSensor::set_template(std::function<optional<float>()> &&f) {
  this->f_ = std::move(f);
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SENSOR
