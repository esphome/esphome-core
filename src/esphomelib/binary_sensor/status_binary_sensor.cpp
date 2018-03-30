//
//  status_binary_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 29.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/binary_sensor/status_binary_sensor.h"

namespace esphomelib {

std::string binary_sensor::StatusBinarySensor::device_class() {
  return "connectivity";
}

} // namespace esphomelib
