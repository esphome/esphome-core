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
  PPD42XSensor(const std::string &name, GPIOInputPin &pl, PPD42XSensorType type);

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  const PPD42XSensorType type_;
  const GPIOInputPin pl_;
};

class PPD42XComponent : public Component {
 public:
  PPD42XComponent(uint32_t update_interval, PPD42XType type);

  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  PPD42XSensor *make_pl_02_5_sensor(const std::string &name, const GPIOInputPin &pl);
  PPD42XSensor *make_pl_10_0_sensor(const std::string &name, const GPIOInputPin &pl);

 protected:
  optional<bool> check_byte_();
  void parse_data_();
  uint16_t get_16_bit_uint_(uint8_t start_index);

  uint8_t data_[64];
  uint8_t data_index_{0};
  uint32_t last_transmission_{0};
  const PPD42XType type_;
  PPD42XSensor *pm_2_5_sensor_{nullptr};
  PPD42XSensor *pm_10_0_sensor_{nullptr};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X

#endif  // ESPHOME_SENSOR_PPD42X_H
