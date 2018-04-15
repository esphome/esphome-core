//
//  status_binary_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 29.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_STATUS_BINARY_SENSOR_H
#define ESPHOMELIB_STATUS_BINARY_SENSOR_H

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_STATUS_BINARY_SENSOR

namespace esphomelib {

namespace binary_sensor {

/** Simple binary sensor that reports the online/offline state of the node.
 *
 * Most of the magic doesn't happen here, but in Application.make_status_binary_sensor.
 */
class StatusBinarySensor : public BinarySensor {
 public:
  explicit StatusBinarySensor(const std::string &name);

  std::string device_class() override;
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //USE_STATUS_BINARY_SENSOR

#endif //ESPHOMELIB_STATUS_BINARY_SENSOR_H
