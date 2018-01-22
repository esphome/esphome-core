//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_SWITCH_PLATFORM_SWITCH_H
#define ESPHOMELIB_SWITCH_PLATFORM_SWITCH_H

#include "esphomelib/binary_sensor/binary_sensor.h"
namespace esphomelib {

namespace switch_platform {

/** Base class for all switches.
 *
 * A switch is basically just a combination of a binary sensor (for reporting switch values)
 * and a write_state method that writes a state to the hardware.
 */
class Switch : public binary_sensor::BinarySensor {
 public:
  Switch();

  binary_sensor::binary_callback_t create_write_state_callback();

  /// Override this method to write the provided state to hardware.
  virtual void write_state(bool state) = 0;
};

} // namespace switch_platform

} // namespace esphomelib

#endif //ESPHOMELIB_SWITCH_PLATFORM_SWITCH_H
