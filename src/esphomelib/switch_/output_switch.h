#ifndef ESPHOMELIB_SWITCH_OUTPUT_SWITCH
#define ESPHOMELIB_SWITCH_OUTPUT_SWITCH

#include "esphomelib/defines.h"

#ifdef USE_OUTPUT_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

/// A simple switch that exposes a binary output as a switch.
class OutputSwitch : public Switch {
 public:
  /// Construct this SimpleSwitch with the provided BinaryOutput.
  explicit OutputSwitch(const std::string &name, output::BinaryOutput *output);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
 protected:
  /// Turn this SimpleSwitch on.
  void turn_on() override;
  /// Turn this SimpleSwitch off.
  void turn_off() override;

  output::BinaryOutput *output_;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_OUTPUT_SWITCH

#endif //ESPHOMELIB_SWITCH_OUTPUT_SWITCH
