//
//  bmp085_component.h
//  esphomelib
//
//  Created by Otto Winter on 27.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_BMP085_COMPONENT_H
#define ESPHOMELIB_SENSOR_BMP085_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_BMP085_SENSOR

#include "BMP085.h"

namespace esphomelib {

namespace sensor {

using BMP085TemperatureSensor = sensor::EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using BMP085PressureSensor = sensor::EmptyPollingParentSensor<1, ICON_GAUGE, UNIT_HPA>;

/** This Component represents a BMP085/BMP180/BMP280 Pressure+Temperature i2c sensor.
 *
 * It's built up similar to the DHT component: a central hub that has two sensors.
 */
class BMP085Component : public PollingComponent {
 public:
  /** Construct the BMP085Component using the provided address and update interval.
   *
   * @param address The i2c address of the sensor, defaults to 0x77.
   * @param update_interval The interval in ms to check the sensor.
   */
  explicit BMP085Component(const std::string &temperature_name, const std::string &pressure_name,
                           uint32_t update_interval = 30000);

  /// Set the i2c address of this sensor.
  void set_address(uint8_t address);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get a handle to the internal i2cdevlib BMP085 object.
  BMP085 &get_bmp();

  /// Get the internal temperature sensor used to expose the temperature as a sensor object.
  BMP085TemperatureSensor *get_temperature_sensor() const;
  /// Get the internal pressure sensor used to expose the pressure as a sensor object.
  BMP085PressureSensor *get_pressure_sensor() const;

  /// Schedule temperature+pressure readings.
  void update() override;
  /// Setup the sensor and test for a connection.
  void setup() override;

 protected:
  /// Internal method to read the temperature from the component after it has been scheduled.
  void read_temperature_();
  /// Internal method to read the pressure from the component after it has been scheduled.
  void read_pressure_();

  BMP085 bmp_;
  uint8_t address_;
  enum { IDLE, TEMPERATURE, PRESSURE } measurement_mode_{IDLE}; ///< The mode the sensor is currently in.
  BMP085TemperatureSensor *temperature_{nullptr};
  BMP085PressureSensor *pressure_{nullptr};
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_BMP085_SENSOR

#endif //ESPHOMELIB_SENSOR_BMP085_COMPONENT_H
