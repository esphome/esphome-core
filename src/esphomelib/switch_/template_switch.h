//
//  template_switch.h
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H
#define ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H

#include "esphomelib/switch_/switch.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

class TemplateSwitch : public Switch {
 public:
  explicit TemplateSwitch(const std::string &name);

  void set_state_lambda(std::function<optional<bool>()> &&f);
  Trigger<NoArg> *get_turn_on_trigger() const;
  Trigger<NoArg> *get_turn_off_trigger() const;
  void set_optimistic(bool optimistic);
  void loop() override;

  float get_setup_priority() const override;

  bool do_restore_state() override;

 protected:
  bool optimistic() override;

  void turn_on() override;
  void turn_off() override;

  optional<std::function<optional<bool>()>> f_;
  bool optimistic_{false};
  optional<bool> last_value_{};
  Trigger<NoArg> *turn_on_trigger_;
  Trigger<NoArg> *turn_off_trigger_;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SWITCH

#endif //ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H
