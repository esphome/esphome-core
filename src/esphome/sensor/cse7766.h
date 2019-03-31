#ifndef ESPHOME_SENSOR_CSE7766_H
#define ESPHOME_SENSOR_CSE7766_H

#include "esphome/defines.h"

#ifdef USE_CSE7766

#include "esphome/component.h"
#include "esphome/uart_component.h"
#include "esphome/helpers.h"
#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using CSE7766VoltageSensor = EmptySensor<1, ICON_FLASH, UNIT_V>;
using CSE7766CurrentSensor = EmptySensor<1, ICON_FLASH, UNIT_A>;
using CSE7766PowerSensor = EmptySensor<1, ICON_FLASH, UNIT_W>;

class CSE7766Component : public PollingComponent, public UARTDevice {
 public:
  CSE7766Component(UARTComponent *parent, uint32_t update_interval = 60000);

  CSE7766VoltageSensor *make_voltage_sensor(const std::string &name);

  CSE7766CurrentSensor *make_current_sensor(const std::string &name);

  CSE7766PowerSensor *make_power_sensor(const std::string &name);

  void loop() override;
  float get_setup_priority() const override;
  void update() override;
  void dump_config() override;

 protected:
  bool check_byte_();
  void parse_data_();
  uint32_t get_24_bit_uint_(uint8_t start_index);

  uint8_t raw_data_[24];
  uint8_t raw_data_index_{0};
  uint32_t last_transmission_{0};
  CSE7766VoltageSensor *voltage_sensor_{nullptr};
  CSE7766CurrentSensor *current_sensor_{nullptr};
  CSE7766PowerSensor *power_sensor_{nullptr};
  float voltage_acc_{0.0f};
  float current_acc_{0.0f};
  float power_acc_{0.0f};
  uint32_t voltage_counts_{0};
  uint32_t current_counts_{0};
  uint32_t power_counts_{0};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_CSE7766

#endif  // ESPHOME_SENSOR_CSE7766_H
