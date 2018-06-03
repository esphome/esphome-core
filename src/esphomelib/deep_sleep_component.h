//
//  deep_sleep_component.h
//  esphomelib
//
//  Created by Otto Winter on 15.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_DEEP_SLEEP_COMPONENT_H
#define ESPHOMELIB_DEEP_SLEEP_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_DEEP_SLEEP

ESPHOMELIB_NAMESPACE_BEGIN

/** This component allows setting up the node to go into deep sleep mode to conserve battery.
 *
 * To set this component up, first set *when* the deep sleep should trigger using set_run_cycles
 * and set_run_duration, then set how long the deep sleep should last using set_sleep_duration and optionally
 * on the ESP32 set_wakeup_pin.
 */
class DeepSleepComponent : public Component {
 public:
  /// Set the duration in ms the component should sleep once it's in deep sleep mode.
  void set_sleep_duration(uint32_t time_ms);
#ifdef ARDUINO_ARCH_ESP32
  /** Set the pin to wake up to on the ESP32 once it's in deep sleep mode.
   * Use the inverted property to set the wakeup level.
   */
  void set_wakeup_pin(GPIOInputPin pin);
#endif
  /// Set the number of loop cycles after which the node should go into deep sleep mode.
  void set_run_cycles(uint32_t cycles);
  /// Set a duration in ms for how long the code should run before entering deep sleep mode.
  void set_run_duration(uint32_t time_ms);

  void setup() override;
  void loop() override;
  float get_loop_priority() const override;
  float get_setup_priority() const override;

 protected:
  /// Helper to enter deep sleep mode
  void begin_sleep();

  optional<uint64_t> sleep_duration_;
#ifdef ARDUINO_ARCH_ESP32
  optional<GPIOInputPin> wakeup_pin_;
#endif
  optional<uint32_t> loop_cycles_;
  uint32_t at_loop_cycle_{0};
  optional<uint32_t> run_duration_;
};

ESPHOMELIB_NAMESPACE_END

#endif //USE_DEEP_SLEEP

#endif //ESPHOMELIB_DEEP_SLEEP_COMPONENT_H
