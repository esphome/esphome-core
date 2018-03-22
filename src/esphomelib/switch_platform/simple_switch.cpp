//
// Created by Otto Winter on 20.01.18.
//

#include "esphomelib/switch_platform/simple_switch.h"

#include "esphomelib/log.h"

namespace esphomelib {

namespace switch_platform {

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

} // namespace switch_platform

} // namespace esphomelib
