#include "esphomelib/defines.h"

#ifdef USE_OUTPUT_SWITCH

#include "esphomelib/switch_/output_switch.h"

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

OutputSwitch::OutputSwitch(const std::string &name, output::BinaryOutput *output)
    : Switch(name), output_(output) { }

void OutputSwitch::turn_on() {
  this->output_->enable();
  this->publish_state(true);
}
void OutputSwitch::turn_off() {
  this->output_->disable();
  this->publish_state(false);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_OUTPUT_SWITCH
