//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/switch_/switch.h"

#ifdef USE_SWITCH

namespace esphomelib {

namespace switch_ {

std::string Switch::icon() {
  return "";
}
Switch::Switch(const std::string &name) : BinarySensor(name) {}

} // namespace switch_

} // namespace esphomelib

#endif //USE_SWITCH
