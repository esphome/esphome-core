#ifndef ESPHOMELIB_SENSOR_BMP085_COMPONENT_H
#define ESPHOMELIB_SENSOR_BMP085_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_BMP085_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/i2c_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using BMP085TemperatureSensor = sensor::EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using BMP085PressureSensor = sensor::EmptyPollingParentSensor<1, ICON_GAUGE, UNIT_HPA>;

/** This Component represents a BMP085/BMP180/BMP280 Pressure+Temperature i2c sensor.
 *
 * It's built up similar to the DHT component: a central hub that has two sensors.
 */
class BMP085Component : public PollingComponent, public I2CDevice {
 public:
  /// Construct the BMP085Component using the provided address and update interval.
  BMP085Component(I2CComponent *parent,
                  const std::string &temperature_name, const std::string &pressure_name,
                  uint32_t update_interval = 30000);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get the internal temperature sensor used to expose the temperature as a sensor object.
  BMP085TemperatureSensor *get_temperature_sensor() const;
  /// Get the internal pressure sensor used to expose the pressure as a sensor object.
  BMP085PressureSensor *get_pressure_sensor() const;

  /// Schedule temperature+pressure readings.
  void update() override;
  /// Setup the sensor and test for a connection.
  void setup() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  struct CalibrationData {
    int16_t ac1, ac2, ac3;
    uint16_t ac4, ac5, ac6;
    int16_t b1, b2;
    int16_t mb, mc, md;
    float temp;
  };

  /// Internal method to read the temperature from the component after it has been scheduled.
  void read_temperature_();
  /// Internal method to read the pressure from the component after it has been scheduled.
  void read_pressure_();

  bool set_mode_(uint8_t mode);

  BMP085TemperatureSensor *temperature_{nullptr};
  BMP085PressureSensor *pressure_{nullptr};
  CalibrationData calibration_;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BMP085_SENSOR

#endif //ESPHOMELIB_SENSOR_BMP085_COMPONENT_H
