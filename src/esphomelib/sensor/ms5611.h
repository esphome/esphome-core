//
//  ms5611.h
//  esphomelib
//
//  Created by Otto Winter on 07.08.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_MS5611_H
#define ESPHOMELIB_SENSOR_MS5611_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/i2c_component.h"
#include "esphomelib/defines.h"

#ifdef USE_MS5611

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using MS5611TemperatureSensor = sensor::EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using MS5611PressureSensor = sensor::EmptyPollingParentSensor<1, ICON_GAUGE, UNIT_HPA>;

class MS5611Component : public PollingComponent, public I2CDevice {
 public:
  MS5611Component(I2CComponent *parent, const std::string &temperature_name, const std::string &pressure_name,
                  uint32_t update_interval = 15000);

  void setup() override;
  float get_setup_priority() const override;
  void update() override;

  MS5611TemperatureSensor *get_temperature_sensor() const;
  MS5611PressureSensor *get_pressure_sensor() const;

 protected:
  void read_temperature_();
  void read_pressure_(uint32_t raw_temperature);
  void calculate_values(uint32_t raw_temperature, uint32_t raw_pressure);

  MS5611TemperatureSensor *temperature_sensor_;
  MS5611PressureSensor *pressure_sensor_;
  uint16_t prom[6];
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MS5611

#endif //ESPHOMELIB_SENSOR_MS5611_H
