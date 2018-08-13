//
//  template_binary_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_BINARY_SENSOR

#include "esphomelib/binary_sensor/template_binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

TemplateBinarySensor::TemplateBinarySensor(const std::string &name)
    : BinarySensor(name) {

}
void TemplateBinarySensor::loop() {
  auto s = this->f_();
  if (s.has_value()) {
    this->publish_state(*s);
  }
}
float TemplateBinarySensor::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
void TemplateBinarySensor::set_template(std::function<optional<bool>()> &&f) {
  this->f_ = std::move(f);
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_BINARY_SENSOR

