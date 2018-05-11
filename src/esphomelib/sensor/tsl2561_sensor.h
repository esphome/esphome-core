//
//  tsl2561_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 09.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_TSL2561_SENSOR_H
#define ESPHOMELIB_SENSOR_TSL2561_SENSOR_H

#include "esphomelib/i2c_component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_TSL2561

namespace esphomelib {

namespace sensor {

enum TSL2561IntegrationTime {
  TSL2561_INTEGRATION_14MS = 0b00,
  TSL2561_INTEGRATION_101MS = 0b01,
  TSL2561_INTEGRATION_402MS = 0b10,
};

enum TSL2561Gain {
  TSL2561_GAIN_1X = 0,
  TSL2561_GAIN_16X = 1,
};

class TSL2561Sensor : public PollingSensorComponent, public I2CDevice {
 public:
  TSL2561Sensor(I2CComponent *parent, const std::string &name, uint8_t address = 0x39,
                uint32_t update_interval = 15000);

  void set_integration_time(TSL2561IntegrationTime integration_time);
  void set_gain(TSL2561Gain gain);
  void set_is_cs_package(bool package_cs);
  void setup() override;
  void update() override;
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

  float get_setup_priority() const override;

 protected:
  float get_integration_time_ms_();
  void read_data_();
  float calculate_lx_(uint16_t ch0, uint16_t ch1);

  TSL2561IntegrationTime integration_time_{TSL2561_INTEGRATION_402MS};
  TSL2561Gain gain_{TSL2561_GAIN_1X};
  bool package_cs_{false};
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_TSL2561

#endif //ESPHOMELIB_SENSOR_TSL2561_SENSOR_H
