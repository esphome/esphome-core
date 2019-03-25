#ifndef ESPHOME_CLIMATEDEVICE_THERMOSTAT_CLIMATEDEVICE_H
#define ESPHOME_CLIMATEDEVICE_THERMOSTAT_CLIMATEDEVICE_H

#include "esphome/defines.h"

#ifdef USE_THERMOSTAT_CLIMATEDEVICE

#include "esphome/climatedevice/climatedevice.h"
#include "esphome/output/binary_output.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

class ThermostatClimateDevice : public ClimateDevice {
 public:
  explicit ThermostatClimateDevice(const std::string &name, output::BinaryOutput *output,
                                   uint32_t update_interval = 60000);

  /// Set whether this thermostat is a cooling device or a heating device. Defaults to false.
  void set_cooling(bool cooling);
  /// Set hysteresis parameter of the thermostat. Defaults to cold = hot = 0.3.
  void set_hysteresis(float cold, float hot);
  /// Set hysteresis parameter of the thermostat. Same for hot and cold. Defaults to 0.3.
  void set_hysteresis(float hysteresis);
  ///  Set a minimum amount of time (in ms) that the output must be in its current state prior to being switched either
  ///  off or on. Defaults to 0 ms.
  void set_min_cycle_duration(uint32_t min_cycle_duration);

  void setup() override;
  void update() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  void control_(float error_value);

  output::BinaryOutput *output_;
  bool output_state_{false};
  bool cooling_{false};
  float hysteresis_hot_{0.3};
  float hysteresis_cold_{0.3};
  uint32_t min_cycle_duration_{0};
  uint32_t last_output_cycle_time_;
};

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_THERMOSTAT_CLIMATEDEVICE

#endif  // ESPHOME_CLIMATEDEVICE_THERMOSTAT_CLIMATEDEVICE_H
