//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/switch_platform/switch.h"

namespace esphomelib {

namespace switch_platform {

binary_sensor::binary_callback_t Switch::create_on_set_state_callback() {
  return [&](bool state) {
    if (state) this->turn_on();
    else this->turn_off();
  };
}
Switch::Switch() : BinarySensor() {}

} // namespace switch_platform

} // namespace esphomelib
