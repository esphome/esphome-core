//
//  template_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 19.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/template_sensor.h"

#ifdef USE_TEMPLATE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

TemplateSensor::TemplateSensor(const std::string &name, std::function<optional<float>()> &&f, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), f_(std::move(f)) {

}
void TemplateSensor::update() {
  auto val = this->f_();
  if (val.has_value()) {
    this->push_new_value(*val);
  }
}
float TemplateSensor::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SENSOR
