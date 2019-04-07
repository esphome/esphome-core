#ifndef ESPHOME_SENSOR_PPD42X_H
#define ESPHOME_SENSOR_PPD42X_H

#include "esphome/defines.h"

#ifdef USE_PPD42X

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/uart_component.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

enum PPD42XType {
  PPD42X_TYPE_X003 = 0,
  PPD42X_TYPE_5003T,
  PPD42X_TYPE_5003ST,
};

enum PPD42XSensorType {
  /// PM1.0 concentration in µg/m^3, PPD42X
  PPD42X_SENSOR_TYPE_PM_1_0 = 0,
  /// PM2.5 concentration in µg/m^3, PPD42X, PMS5003T, PMS5003ST
  PPD42X_SENSOR_TYPE_PM_2_5,
  /// PM10.0 concentration in µg/m^3, PPD42X
  PPD42X_SENSOR_TYPE_PM_10_0,
  /// Temperature in °C, PMS5003T, PMS5003ST
  PPD42X_SENSOR_TYPE_TEMPERATURE,
  /// Relative Humidity in %, PMS5003T, PMS5003T
  PPD42X_SENSOR_TYPE_HUMIDITY,
  /// Formaldehyde in µg/m^3, PMS5003ST
  PPD42X_SENSOR_TYPE_FORMALDEHYDE,
};

class PPD42XSensor : public sensor::Sensor {
 public:
  PPD42XSensor(const std::string &name, PPD42XSensorType type);

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  const PPD42XSensorType type_;
};

class PPD42XComponent : public UARTDevice, public Component {
 public:
  PPD42XComponent(UARTComponent *parent, PPD42XType type);

  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  PPD42XSensor *make_pm_1_0_sensor(const std::string &name);
  PPD42XSensor *make_pm_2_5_sensor(const std::string &name);
  PPD42XSensor *make_pm_10_0_sensor(const std::string &name);
  PPD42XSensor *make_temperature_sensor(const std::string &name);
  PPD42XSensor *make_humidity_sensor(const std::string &name);
  PPD42XSensor *make_formaldehyde_sensor(const std::string &name);

 protected:
  optional<bool> check_byte_();
  void parse_data_();
  uint16_t get_16_bit_uint_(uint8_t start_index);

  uint8_t data_[64];
  uint8_t data_index_{0};
  uint32_t last_transmission_{0};
  const PPD42XType type_;
  PPD42XSensor *pm_1_0_sensor_{nullptr};
  PPD42XSensor *pm_2_5_sensor_{nullptr};
  PPD42XSensor *pm_10_0_sensor_{nullptr};
  PPD42XSensor *temperature_sensor_{nullptr};
  PPD42XSensor *humidity_sensor_{nullptr};
  PPD42XSensor *formaldehyde_sensor_{nullptr};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X

#endif  // ESPHOME_SENSOR_PPD42X_H
