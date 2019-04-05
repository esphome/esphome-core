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
  PPD42X_TYPE___ = 0,
  PPD42X_TYPE_NS,
};

enum PPD42XSensorType {
  /// PM2.5 concentration in pcs/L, PPD42, PPD42NS
  PPD42X_SENSOR_TYPE_PM_02_5,
  /// PM10.0 concentration in pcs/L, PPD42, PPD42NS
  PPD42X_SENSOR_TYPE_PM_10_0,

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

class PPD42XComponent : public Component {
 public:
  PPD42XComponent(PPD42XType type);

  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  PPD42XSensor *make_pm_02_5_sensor(const std::string &name, GPIOPin *pm_pin);
  PPD42XSensor *make_pm_10_0_sensor(const std::string &name, GPIOPin *pm_pin);

 protected:
  void parse_data_();
  const PPD42XType type_;
  PPD42XSensor *pm_02_5_sensor_{nullptr};
  PPD42XSensor *pm_10_0_sensor_{nullptr};
  /// Helper function to convert the specified pm_xx_x duration in µs to pcs/L.
  static float us_to_pm(uint32_t sample_length, uint32_t time_pm);
  /// Helper function to convert the specified distance in meters to the pm_10_0 duration in µs.
  uint32_t timeout_us_{30000};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X

#endif  // ESPHOME_SENSOR_PPD42X_H
