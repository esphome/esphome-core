//
//  template_binary_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/binary_sensor/template_binary_sensor.h"

#ifdef USE_TEMPLATE_BINARY_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

TemplateBinarySensor::TemplateBinarySensor(const std::string &name, std::function<optional<bool>()> &&f)
    : BinarySensor(name), f_(std::move(f)) {

}
void TemplateBinarySensor::loop() {
  auto s = this->f_();
  if (s.has_value()) {
    this->publish_state(*s);
  }
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_BINARY_SENSOR

