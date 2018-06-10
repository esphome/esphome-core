//
//  template_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 19.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_TEMPLATE_SENSOR_H
#define ESPHOMELIB_SENSOR_TEMPLATE_SENSOR_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class TemplateSensor : public PollingSensorComponent {
 public:
  TemplateSensor(const std::string &name, std::function<optional<float>()> &&f,
                 uint32_t update_interval = 15000);

  void update() override;

  float get_setup_priority() const override;

 protected:
  std::function<optional<float>()> f_;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SENSOR

#endif //ESPHOMELIB_SENSOR_TEMPLATE_SENSOR_H
