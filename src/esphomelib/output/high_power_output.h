//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_OUTPUT_HIGH_POWER_OUTPUT_H
#define ESPHOMELIB_OUTPUT_HIGH_POWER_OUTPUT_H

#include "esphomelib/power_supply_component.h"

namespace esphomelib {

namespace output {

/// Enable outputs to automatically power on a power supply.
class HighPowerOutput {
 public:
  /// Defaults to no power supply (nullptr).
  HighPowerOutput();

  PowerSupplyComponent *get_power_supply() const;
  /// Set an PowerSupplyComponent that will be enabled if power is needed by sub-classes.
  void set_power_supply(PowerSupplyComponent *power_supply);

  /// Use this method to enable the power supply.
  void enable_power_supply();

 protected:
  PowerSupplyComponent *power_supply_;
};

} // namespace output

} // namespace esphomelib

#endif //ESPHOMELIB_OUTPUT_HIGH_POWER_OUTPUT_H
