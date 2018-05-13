//
//  restart_switch.h
//  esphomelib
//
//  Created by Otto Winter on 29.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_RESTART_SWITCH_H
#define ESPHOMELIB_RESTART_SWITCH_H

#include "esphomelib/switch_/switch.h"
#include "esphomelib/defines.h"

#ifdef USE_RESTART_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

/// A simple switch that restarts the device when triggered.
class RestartSwitch : public Switch {
 public:
  explicit RestartSwitch(const std::string &name);

  void turn_on() override;
  void turn_off() override;
  std::string icon() override;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_RESTART_SWITCH

#endif //ESPHOMELIB_RESTART_SWITCH_H
