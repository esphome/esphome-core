//
//  bh1750_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 10.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_BH1750_SENSOR_H
#define ESPHOMELIB_SENSOR_BH1750_SENSOR_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/i2c_component.h"
#include "esphomelib/defines.h"

#ifdef USE_BH1750

namespace esphomelib {

namespace sensor {

enum BH1750Resolution {
  BH1750_RESOLUTION_4P0_LX = 0b00100011, // one-time low resolution mode
  BH1750_RESOLUTION_1P0_LX = 0b00100000, // one-time high resolution mode 1
  BH1750_RESOLUTION_0P5_LX = 0b00100001, // one-time high resolution mode 2
};

class BH1750Sensor : public PollingSensorComponent, public I2CDevice {
 public:
  BH1750Sensor(I2CComponent *parent, const std::string &name,
               uint8_t address = 0x23, uint32_t update_interval = 15000);

  void set_resolution(BH1750Resolution resolution);

  void setup() override;
  void update() override;
  float get_setup_priority() const override;
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  void read_data_();

  BH1750Resolution resolution_{BH1750_RESOLUTION_0P5_LX};
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_BH1750

#endif //ESPHOMELIB_SENSOR_BH1750_SENSOR_H
