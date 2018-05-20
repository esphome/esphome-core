//
//  template_binary_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H

#include "esphomelib/component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_BINARY_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

class TemplateBinarySensor : public Component, public BinarySensor {
 public:
  TemplateBinarySensor(const std::string &name, std::function<optional<bool>()> &&f);

  void loop() override;

 protected:
  std::function<optional<bool>()> &&f_;
};

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_TEMPLATE_BINARY_SENSOR_H
