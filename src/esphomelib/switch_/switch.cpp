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

std::string Switch::get_icon() {
  if (this->icon_.defined)
    return this->icon_.value;
  return this->icon();
}

void Switch::set_icon(const std::string &icon) {
  this->icon_ = icon;
}

} // namespace switch_

} // namespace esphomelib

#endif //USE_SWITCH
