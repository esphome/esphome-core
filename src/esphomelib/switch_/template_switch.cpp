//
//  template_switch.cpp
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SWITCH

#include "esphomelib/switch_/template_switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

TemplateSwitch::TemplateSwitch(const std::string &name)
    : Switch(name), turn_on_trigger_(new Trigger<NoArg>()), turn_off_trigger_(new Trigger<NoArg>()) {

}
void TemplateSwitch::loop() {
  if (this->f_.has_value()) {
    auto s = (*this->f_)();
    if (s.has_value())
      this->publish_state(*s);
  }
}
void TemplateSwitch::turn_on() {
  if (this->optimistic_)
    this->publish_state(true);
  this->turn_on_trigger_->trigger();
}
void TemplateSwitch::turn_off() {
  if (this->optimistic_)
    this->publish_state(false);
  this->turn_off_trigger_->trigger();
}
void TemplateSwitch::set_optimistic(bool optimistic) {
  this->optimistic_ = optimistic;
}
bool TemplateSwitch::optimistic() {
  return this->optimistic_;
}
void TemplateSwitch::set_state_lambda(std::function<optional<bool>()> &&f) {
  this->f_ = f;
}
float TemplateSwitch::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
Trigger<NoArg> *TemplateSwitch::get_turn_on_trigger() const {
  return this->turn_on_trigger_;
}
Trigger<NoArg> *TemplateSwitch::get_turn_off_trigger() const {
  return this->turn_off_trigger_;
}
bool TemplateSwitch::do_restore_state() {
  return false;
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SWITCH
