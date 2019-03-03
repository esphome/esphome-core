#ifndef ESPHOME_SENSOR_PMSX003_H
#define ESPHOME_SENSOR_PMSX003_H

#include "esphome/defines.h"

#ifdef USE_PMSX003

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/uart_component.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

enum PMSX003Type {
  PMSX003_TYPE_X003 = 0,
  PMSX003_TYPE_5003T,
  PMSX003_TYPE_5003ST,
};

enum PMSX003SensorType {
  /// PM1.0 concentration in µg/m^3, PMSX003
  PMSX003_SENSOR_TYPE_PM_1_0 = 0,
  /// PM2.5 concentration in µg/m^3, PMSX003, PMS5003T, PMS5003ST
  PMSX003_SENSOR_TYPE_PM_2_5,
  /// PM10.0 concentration in µg/m^3, PMSX003
  PMSX003_SENSOR_TYPE_PM_10_0,
  /// Temperature in °C, PMS5003T, PMS5003ST
  PMSX003_SENSOR_TYPE_TEMPERATURE,
  /// Relative Humidity in %, PMS5003T, PMS5003T
  PMSX003_SENSOR_TYPE_HUMIDITY,
  /// Formaldehyde in µg/m^3, PMS5003ST
  PMSX003_SENSOR_TYPE_FORMALDEHYDE,
};

class PMSX003Sensor : public sensor::Sensor {
 public:
  PMSX003Sensor(const std::string &name, PMSX003SensorType type);

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  const PMSX003SensorType type_;
};

class PMSX003Component : public UARTDevice, public Component {
 public:
  PMSX003Component(UARTComponent *parent, PMSX003Type type);

  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  PMSX003Sensor *make_pm_1_0_sensor(const std::string &name);
  PMSX003Sensor *make_pm_2_5_sensor(const std::string &name);
  PMSX003Sensor *make_pm_10_0_sensor(const std::string &name);
  PMSX003Sensor *make_temperature_sensor(const std::string &name);
  PMSX003Sensor *make_humidity_sensor(const std::string &name);
  PMSX003Sensor *make_formaldehyde_sensor(const std::string &name);

 protected:
  optional<bool> check_byte_();
  void parse_data_();
  uint16_t get_16_bit_uint_(uint8_t start_index);

  uint8_t data_[64];
  uint8_t data_index_{0};
  uint32_t last_transmission_{0};
  const PMSX003Type type_;
  PMSX003Sensor *pm_1_0_sensor_{nullptr};
  PMSX003Sensor *pm_2_5_sensor_{nullptr};
  PMSX003Sensor *pm_10_0_sensor_{nullptr};
  PMSX003Sensor *temperature_sensor_{nullptr};
  PMSX003Sensor *humidity_sensor_{nullptr};
  PMSX003Sensor *formaldehyde_sensor_{nullptr};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PMSX003

#endif  // ESPHOME_SENSOR_PMSX003_H
