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
  PPD42X_TYPE = 0,
  PPD42X_TYPE_NJ,
  PPD42X_TYPE_NS,
};

enum PPD42XSensorType {
  /// PM2.5 concentration in pcs/L, PPD42, PPD42NJ, PPD42NS
  PPD42X_SENSOR_TYPE_PM_02_5,
  /// PM10.0 concentration in pcs/L, PPD42X
  PPD42X_SENSOR_TYPE_PM_10_0,
};

class PPD42XSensor : public sensor::Sensor {
 public:
  PPD42XSensor(const std::string &name, GPIOInputPin *pl, PPD42XSensorType type);

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  GPIOInputPin *pl_pin_;

 protected:
  const PPD42XSensorType stype_;

};  // class PPD42XSensor

class PPD42XComponent : public Component {
 public:
  PPD42XComponent(PPD42XType type, uint32_t update_interval, uint32_t time_out);
  void set_timeout_us(uint32_t timeout_us);
  void update() override;
  float get_setup_priority() const override;
  void dump_config() override;

  PPD42XSensor *make_pl_02_5_sensor(const std::string &name, GPIOInputPin *pl);
  PPD42XSensor *make_pl_10_0_sensor(const std::string &name, GPIOInputPin *pl);

 protected:
  void parse_data_();
  static float us_to_pl(uint32_t sample_length, uint32_t time_pm);

  uint32_t timeout_ms_{30};
  uint32_t starttime_{0};
  uint32_t lowpulseoccupancy_02_5_{0};
  uint32_t lowpulseoccupancy_10_0_{0};

  uint32_t last_transmission_{0};
  uint32_t ui_{0};
  const PPD42XType ctype_;
  PPD42XSensor *pl_02_5_sensor_{nullptr};
  PPD42XSensor *pl_10_0_sensor_{nullptr};
};  // class PPD42XComponent

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X

#endif  // ESPHOME_SENSOR_PPD42X_H
