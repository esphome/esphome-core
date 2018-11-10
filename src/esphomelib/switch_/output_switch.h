#ifndef ESPHOMELIB_SWITCH_OUTPUT_SWITCH
#define ESPHOMELIB_SWITCH_OUTPUT_SWITCH

#include "esphomelib/defines.h"

#ifdef USE_OUTPUT_SWITCH

#include "esphomelib/output/binary_output.h"
#include "esphomelib/switch_/switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

/// A simple switch that exposes a binary output as a switch.
class OutputSwitch : public Switch, public Component {
 public:
  /// Construct this SimpleSwitch with the provided BinaryOutput.
  explicit OutputSwitch(const std::string &name, output::BinaryOutput *output);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  void setup() override;
  float get_setup_priority() const override;
 protected:
  void write_state(bool state) override;

  output::BinaryOutput *output_;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_OUTPUT_SWITCH

#endif //ESPHOMELIB_SWITCH_OUTPUT_SWITCH
