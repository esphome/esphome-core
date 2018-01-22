//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_OUTPUT_HIGH_POWER_OUTPUT_H
#define ESPHOMELIB_OUTPUT_HIGH_POWER_OUTPUT_H

#include <esphomelib/atx_component.h>
namespace esphomelib {

namespace output {

/// Enable outputs to automatically power on a power supply.
class HighPowerOutput {
 public:
  /// Defaults to no ATX power supply (nullptr).
  HighPowerOutput();

  ATXComponent *get_atx() const;
  /// Set an ATXComponent that will be enabled if power is needed by sub-classes.
  void set_atx(ATXComponent *atx);

  /// Use this method to enable the ATX power supply.
  void enable_atx();

 protected:
  ATXComponent *atx_;
};

} // namespace output

} // namespace esphomelib

#endif //ESPHOMELIB_OUTPUT_HIGH_POWER_OUTPUT_H
