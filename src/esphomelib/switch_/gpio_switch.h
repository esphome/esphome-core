//
//  gpio_switch.h
//  esphomelib
//
//  Created by Otto Winter on 2018/10/15.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SWITCH_GPIO_SWITCH_H
#define ESPHOMELIB_SWITCH_GPIO_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_GPIO_SWITCH

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/switch_/switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

class GPIOSwitch : public Switch {
 public:
  GPIOSwitch(const std::string &name, GPIOPin *pin);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  float get_setup_priority() const override;

  void setup() override;

  void set_power_on_value(bool power_on_value);

 protected:
  void turn_on() override;
  void turn_off() override;

  GPIOPin *pin_;
  bool power_on_value_{false};
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_SWITCH

#endif //ESPHOMELIB_SWITCH_GPIO_SWITCH_H
