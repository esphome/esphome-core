//
//  bme680_component.h
//  esphomelib
//
//  Created by Otto Winter on 10.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_BME680_COMPONENT_H
#define ESPHOMELIB_SENSOR_BME680_COMPONENT_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/i2c_component.h"
#include "esphomelib/defines.h"

#ifdef USE_BME680

namespace esphomelib {

namespace sensor {

extern float BME680_GAS_LOOKUP_TABLE_1[16] PROGMEM;
extern float BME680_GAS_LOOKUP_TABLE_2[16] PROGMEM;

enum BME680IIRFilter {
  BME680_IIR_FILTER_OFF = 0b000,
  BME680_IIR_FILTER_1X = 0b001,
  BME680_IIR_FILTER_3X = 0b010,
  BME680_IIR_FILTER_7X = 0b011,
  BME680_IIR_FILTER_15X = 0b100,
  BME680_IIR_FILTER_31X = 0b101,
  BME680_IIR_FILTER_63X = 0b110,
  BME680_IIR_FILTER_127X = 0b111,
};

enum BME680Oversampling {
  BME680_OVERSAMPLING_NONE = 0b000,
  BME680_OVERSAMPLING_1X = 0b001,
  BME680_OVERSAMPLING_2X = 0b010,
  BME680_OVERSAMPLING_4X = 0b011,
  BME680_OVERSAMPLING_8X = 0b100,
  BME680_OVERSAMPLING_16X = 0b101,
};

struct BME680CalibrationData {
  uint16_t t1;
  uint16_t t2;
  uint8_t t3;

  uint16_t p1;
  int16_t p2;
  int8_t p3;
  int16_t p4;
  int16_t p5;
  int8_t p6;
  int8_t p7;
  int16_t p8;
  int16_t p9;
  int8_t p10;

  uint16_t h1;
  uint16_t h2;
  int8_t h3;
  int8_t h4;
  int8_t h5;
  uint8_t h6;
  int8_t h7;

  int8_t gh1;
  int16_t gh2;
  int8_t gh3;

  uint8_t res_heat_range;
  uint8_t res_heat_val;
  uint8_t range_sw_err;

  float tfine;
  uint8_t ambient_temperature;
};

using BME680TemperatureSensor = sensor::EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using BME680PressureSensor = sensor::EmptyPollingParentSensor<1, ICON_GAUGE, UNIT_HPA>;
using BME680HumiditySensor = sensor::EmptyPollingParentSensor<1, ICON_WATER_PERCENT, UNIT_PERCENT>;
using BME680GasResistanceSensor = sensor::EmptyPollingParentSensor<1, ICON_GAS_CYLINDER, UNIT_OHM>;

class BME680Component : public PollingComponent, public I2CDevice {
 public:
  BME680Component(I2CComponent *parent,
                  const std::string &temperature_name, const std::string &pressure_name,
                  const std::string &humidity_name, const std::string &gas_resistance_name,
                  uint8_t address = 0x76, uint32_t update_interval = 15000);

  void setup() override;
  float get_setup_priority() const override;
  void update() override;

  void set_temperature_oversampling(BME680Oversampling temperature_oversampling);
  void set_pressure_oversampling(BME680Oversampling pressure_oversampling);
  void set_humidity_oversampling(BME680Oversampling humidity_oversampling);
  void set_iir_filter(BME680IIRFilter iir_filter);
  void set_heater(uint16_t heater_temperature, uint16_t heater_duration);

  BME680TemperatureSensor *get_temperature_sensor() const;
  BME680PressureSensor *get_pressure_sensor() const;
  BME680HumiditySensor *get_humidity_sensor() const;
  BME680GasResistanceSensor *get_gas_resistance_sensor() const;

 protected:
  uint8_t calc_heater_resistance_(uint16_t temperature);
  uint8_t calc_heater_duration_(uint16_t duration);
  void read_data_();

  float calc_temperature_(uint32_t raw_temperature);
  float calc_pressure_(uint32_t raw_pressure);
  float calc_humidity_(uint16_t raw_humidity);
  uint32_t calc_gas_resistance_(uint16_t raw_gas, uint8_t range);
  uint32_t calc_meas_duration_();

  BME680CalibrationData calibration_;
  BME680Oversampling temperature_oversampling_{BME680_OVERSAMPLING_16X};
  BME680Oversampling pressure_oversampling_{BME680_OVERSAMPLING_16X};
  BME680Oversampling humidity_oversampling_{BME680_OVERSAMPLING_16X};
  BME680IIRFilter iir_filter_{BME680_IIR_FILTER_OFF};
  uint16_t heater_temperature_{0};
  uint16_t heater_duration_{0};

  BME680TemperatureSensor *temperature_sensor_;
  BME680PressureSensor *pressure_sensor_;
  BME680HumiditySensor *humidity_sensor_;
  BME680GasResistanceSensor *gas_resistance_sensor_;
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_BME680

#endif //ESPHOMELIB_SENSOR_BME680_COMPONENT_H
