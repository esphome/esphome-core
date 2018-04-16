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

namespace esphomelib {

class DeepSleepComponent : public Component {
 public:
  void set_sleep_duration(uint32_t time_ms);
#ifdef ARDUINO_ARCH_ESP32
  void set_wakeup_pin(GPIOInputPin pin);
#endif
  void set_run_cycles(uint32_t cycles);
  void set_run_duration(uint32_t time_ms);

  void setup() override;
  void loop() override;
  float get_loop_priority() const override;

 protected:
  void begin_sleep();

  Optional<uint64_t> sleep_duration_{};
#ifdef ARDUINO_ARCH_ESP32
  Optional<GPIOInputPin> wakeup_pin_{};
#endif
  Optional<uint32_t> loop_cycles_{};
  uint32_t at_loop_cycle_{0};
  Optional<uint32_t> run_duration_{};
};

} // namespace esphomelib

#endif //USE_DEEP_SLEEP

#endif //ESPHOMELIB_DEEP_SLEEP_COMPONENT_H
