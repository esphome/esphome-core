#include "esphomelib/defines.h"

#ifdef USE_OUTPUT_SWITCH

#include "esphomelib/switch_/output_switch.h"

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

OutputSwitch::OutputSwitch(const std::string &name, output::BinaryOutput *output)
    : Switch(name), output_(output) {

}
void OutputSwitch::write_state(bool state) {
  if (state) {
    this->output_->turn_on();
  } else {
    this->output_->turn_off();
  }
  this->publish_state(true);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_OUTPUT_SWITCH
