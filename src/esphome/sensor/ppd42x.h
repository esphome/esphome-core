#ifndef ESPHOME_SENSOR_PPD42X_H
#define ESPHOME_SENSOR_PPD42X_H

#include "esphome/defines.h"

#ifdef USE_PPD42X

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

enum PPD42XType {
  PPD42X_TYPE = 0,
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
  PPD42XSensor(const std::string &name, GPIOPin pl_pin, PPD42XSensorType type);

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  const PPD42XSensorType type_;
  const GPIOPin pl_pin_;
};

class PPD42XComponent : public Component {
 public:
  PPD42XComponent(const GPIOOutputPin &pl_02_5, const GPIOOutputPin &pl_10_0,
                  PPD42XType type);
  void set_timeout_us(uint32_t timeout_us);
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;
  void setup() override;
  void get_update_interval();

  PPD42XSensor *make_pl_02_5_sensor(const std::string &name, GPIOPin pl_pin);
  PPD42XSensor *make_pl_10_0_sensor(const std::string &name, GPIOPin pl_pin);

 protected:
  void parse_data_();
  /// Helper function to convert the specified pl_xx_x duration in µs to pcs/L.
  static float us_to_pl(uint32_t sample_length, uint32_t time_pm);


  /// Helper function to convert the specified distance in meters to the pl_10_0 duration in µs.
  uint32_t timeout_us_{30000};
  uint32_t starttime_{0};
  uint32_t lowpulseoccupancy_02_5_{0};
  uint32_t lowpulseoccupancy_10_0_{0};
  GPIOPin *pl_02_5_pin_;
  GPIOPin *pl_10_0_pin_;
  
  const PPD42XType type_;
  PPD42XSensor *pl_02_5_sensor_{nullptr};
  PPD42XSensor *pl_10_0_sensor_{nullptr};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X

#endif  // ESPHOME_SENSOR_PPD42X_H
