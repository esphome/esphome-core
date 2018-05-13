//
// Created by Otto Winter on 20.01.18.
//

#include "esphomelib/switch_/simple_switch.h"

#include "esphomelib/log.h"

#ifdef USE_SIMPLE_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

SimpleSwitch::SimpleSwitch(const std::string &name, output::BinaryOutput *output)
    : Switch(name), output_(output) { }

void SimpleSwitch::turn_on() {
  assert(this->output_ != nullptr);
  this->output_->enable();

  this->publish_state(true);
}
void SimpleSwitch::turn_off() {
  assert(this->output_ != nullptr);
  this->output_->disable();

  this->publish_state(false);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SIMPLE_SWITCH
