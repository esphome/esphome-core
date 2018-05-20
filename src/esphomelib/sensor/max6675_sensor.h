//
//  max6675.h
//  esphomelib
//
//  Created by Otto Winter on 19.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_MAX6675_H
#define ESPHOMELIB_SENSOR_MAX6675_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_MAX6675_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class MAX6675Sensor : public PollingSensorComponent {
 public:
  MAX6675Sensor(const std::string &name, GPIOPin *cs, GPIOPin *clock, GPIOPin *miso,
                uint32_t update_interval = 15000);

  void setup() override;
  float get_setup_priority() const override;

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  void read_data_();
  uint8_t read_spi_();

  GPIOPin *cs_;
  GPIOPin *clock_;
  GPIOPin *miso_;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MAX6675_SENSOR

#endif //ESPHOMELIB_SENSOR_MAX6675_H
