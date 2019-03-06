#ifndef ESPHOME_INA_219_H
#define ESPHOME_INA_219_H

#include "esphome/defines.h"

#ifdef USE_INA219

#include "esphome/sensor/sensor.h"
#include "esphome/component.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using INA219VoltageSensor = EmptyPollingParentSensor<2, ICON_FLASH, UNIT_V>;
using INA219CurrentSensor = EmptyPollingParentSensor<3, ICON_FLASH, UNIT_A>;
using INA219PowerSensor = EmptyPollingParentSensor<2, ICON_FLASH, UNIT_W>;

class INA219Component : public PollingComponent, public I2CDevice {
 public:
  INA219Component(I2CComponent *parent, float shunt_resistance_ohm, float max_current_a, float max_voltage_v,
                  uint8_t address = 0x40, uint32_t update_interval = 60000);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

  INA219VoltageSensor *make_bus_voltage_sensor(const std::string &name);
  INA219VoltageSensor *make_shunt_voltage_sensor(const std::string &name);
  INA219CurrentSensor *make_current_sensor(const std::string &name);
  INA219PowerSensor *make_power_sensor(const std::string &name);

 protected:
  float shunt_resistance_ohm_;
  float max_current_a_;
  float max_voltage_v_;
  uint32_t calibration_lsb_;
  INA219VoltageSensor *bus_voltage_sensor_{nullptr};
  INA219VoltageSensor *shunt_voltage_sensor_{nullptr};
  INA219CurrentSensor *current_sensor_{nullptr};
  INA219PowerSensor *power_sensor_{nullptr};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_INA219

#endif  // ESPHOME_INA_219_H
