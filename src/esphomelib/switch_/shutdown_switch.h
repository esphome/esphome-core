//
//  shutdown_switch.h
//  esphomelib
//
//  Created by Otto Winter on 04.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SWITCH_SHUTDOWN_SWITCH_H
#define ESPHOMELIB_SWITCH_SHUTDOWN_SWITCH_H

#include "esphomelib/switch_/switch.h"
#include "esphomelib/defines.h"

#ifdef USE_SHUTDOWN_SWITCH

namespace esphomelib {

namespace switch_ {

/// A simple switch that will put the node into deep sleep indefinitely.
class ShutdownSwitch : public Switch {
 public:
  explicit ShutdownSwitch(const std::string &name);

  void turn_on() override;
  void turn_off() override;
  std::string icon() override;
};

} // namespace switch_

} // namespace esphomelib

#endif //USE_RESTART_SWITCH


#endif //ESPHOMELIB_SWITCH_SHUTDOWN_SWITCH_H
