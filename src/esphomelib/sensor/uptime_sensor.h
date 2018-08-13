//
//  uptime_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 25.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_UPTIME_SENSOR_H
#define ESPHOMELIB_UPTIME_SENSOR_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_UPTIME_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class UptimeSensor : public PollingSensorComponent {
 public:
  explicit UptimeSensor(const std::string &name, uint32_t update_interval = 15000);

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;

  float get_setup_priority() const override;

 protected:
  uint64_t uptime_{0};
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_UPTIME_SENSOR

#endif //ESPHOMELIB_UPTIME_SENSOR_H
