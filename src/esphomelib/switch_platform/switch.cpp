//
// Created by Otto Winter on 02.12.17.
//

#include "switch.h"

namespace esphomelib {

namespace switch_platform {

binary_sensor::binary_callback_t Switch::create_write_state_callback() {
  return [&](bool state) { this->write_state(state); };
}
Switch::Switch() : BinarySensor() {}

} // namespace switch_platform

} // namespace esphomelib
