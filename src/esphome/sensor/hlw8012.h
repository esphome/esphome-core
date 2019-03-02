#ifndef ESPHOME_HLW_8012_H
#define ESPHOME_HLW_8012_H

#include "esphome/defines.h"

#ifdef USE_HLW8012

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/sensor/pulse_counter.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class HLW8012Component;

class HLW8012VoltageSensor : public EmptySensor<1, ICON_FLASH, UNIT_V> {
 public:
  HLW8012VoltageSensor(const std::string &name, HLW8012Component *parent);
  uint32_t update_interval() override;

 protected:
  HLW8012Component *parent_;
};
class HLW8012CurrentSensor : public EmptySensor<1, ICON_FLASH, UNIT_A> {
 public:
  HLW8012CurrentSensor(const std::string &name, HLW8012Component *parent);
  uint32_t update_interval() override;

 protected:
  HLW8012Component *parent_;
};
using HLW8012PowerSensor = EmptyPollingParentSensor<1, ICON_FLASH, UNIT_W>;

class HLW8012Component : public PollingComponent {
 public:
  HLW8012Component(GPIOPin *sel_pin, uint8_t cf_pin, uint8_t cf1_pin, uint32_t update_interval = 60000);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

  HLW8012VoltageSensor *make_voltage_sensor(const std::string &name);
  HLW8012CurrentSensor *make_current_sensor(const std::string &name);
  HLW8012PowerSensor *make_power_sensor(const std::string &name);
  void set_change_mode_every(uint32_t change_mode_every);
  void set_current_resistor(float current_resistor);
  void set_voltage_divider(float voltage_divider);

 protected:
  friend HLW8012CurrentSensor;
  friend HLW8012VoltageSensor;

  uint32_t nth_value_{0};
  bool current_mode_{false};
  uint32_t change_mode_at_{0};
  uint32_t change_mode_every_{8};
  float current_resistor_{0.001};
  float voltage_divider_{2351};
  GPIOPin *sel_pin_;
  PulseCounterBase cf_;
  PulseCounterBase cf1_;
  HLW8012VoltageSensor *voltage_sensor_{nullptr};
  HLW8012CurrentSensor *current_sensor_{nullptr};
  HLW8012PowerSensor *power_sensor_{nullptr};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HLW8012

#endif  // ESPHOME_HLW_8012_H
