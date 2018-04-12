//
// Created by Otto Winter on 20.01.18.
//

#include "esphomelib/switch_/simple_switch.h"

#include "esphomelib/log.h"

#ifdef USE_SWITCH

namespace esphomelib {

namespace switch_ {

SimpleSwitch::SimpleSwitch(output::BinaryOutput *output)
    : output_(output) { }

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

} // namespace esphomelib

#endif //USE_SWITCH
