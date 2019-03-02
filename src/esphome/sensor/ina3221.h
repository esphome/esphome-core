#ifndef ESPHOME_INA3221_H
#define ESPHOME_INA3221_H

#include "esphome/defines.h"

#ifdef USE_INA3221

#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using INA3221VoltageSensor = EmptyPollingParentSensor<2, ICON_FLASH, UNIT_V>;
using INA3221CurrentSensor = EmptyPollingParentSensor<2, ICON_FLASH, UNIT_A>;
using INA3221PowerSensor = EmptyPollingParentSensor<2, ICON_FLASH, UNIT_W>;

class INA3221Component : public PollingComponent, public I2CDevice {
 public:
  INA3221Component(I2CComponent *parent, uint8_t address = 0x40, uint32_t update_interval = 60000);
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

  INA3221VoltageSensor *make_bus_voltage_sensor(int channel, const std::string &name);
  INA3221VoltageSensor *make_shunt_voltage_sensor(int channel, const std::string &name);
  INA3221CurrentSensor *make_current_sensor(int channel, const std::string &name);
  INA3221PowerSensor *make_power_sensor(int channel, const std::string &name);
  void set_shunt_resistance(int channel, float resistance_ohm);

 protected:
  struct INA3221Channel {
    float shunt_resistance_{0.1f};
    INA3221VoltageSensor *bus_voltage_sensor_{nullptr};
    INA3221VoltageSensor *shunt_voltage_sensor_{nullptr};
    INA3221CurrentSensor *current_sensor_{nullptr};
    INA3221PowerSensor *power_sensor_{nullptr};

    bool exists();
    bool should_measure_shunt_voltage();
    bool should_measure_bus_voltage();
  } channels_[3];
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_INA3221

#endif  // ESPHOME_INA3221_H
