//
//  template_switch.cpp
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/switch_/template_switch.h"

#ifdef USE_TEMPLATE_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

TemplateSwitch::TemplateSwitch(const std::string &name)
    : Switch(name) {

}
void TemplateSwitch::loop() {
  if (this->f_.has_value()) {
    auto s = (*this->f_)();
    if (s.has_value())
      this->publish_state(*s);
  }
}
void TemplateSwitch::turn_on() {
  this->turn_on_action_.play(false);
  if (this->optimistic_)
    this->publish_state(true);
}
void TemplateSwitch::turn_off() {
  this->turn_off_action_.play(false);
  if (this->optimistic_)
    this->publish_state(false);
}
void TemplateSwitch::add_turn_on_actions(const std::vector<Action<NoArg> *> &actions) {
  this->turn_on_action_.add_actions(actions);
}
void TemplateSwitch::add_turn_off_actions(const std::vector<Action<NoArg> *> &actions) {
  this->turn_off_action_.add_actions(actions);
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

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SWITCH
