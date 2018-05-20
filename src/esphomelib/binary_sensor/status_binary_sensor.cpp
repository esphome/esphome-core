//
//  status_binary_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 29.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/binary_sensor/status_binary_sensor.h"

#ifdef USE_STATUS_BINARY_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

std::string StatusBinarySensor::device_class() {
  return "connectivity";
}
StatusBinarySensor::StatusBinarySensor(const std::string &name)
    : BinarySensor(name) {
  this->value = true;
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_STATUS_BINARY_SENSOR
