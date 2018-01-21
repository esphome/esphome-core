//
// Created by Otto Winter on 30.12.17.
//

#ifndef ESPHOMELIB_FAN_BASIC_FAN_COMPONENT_H
#define ESPHOMELIB_FAN_BASIC_FAN_COMPONENT_H

#include <esphomelib/component.h>
#include <esphomelib/output/binary_output.h>
#include <esphomelib/output/float_output.h>
#include "fan_state.h"

namespace esphomelib {

namespace fan {

class BasicFanComponent : public Component {
 public:
  void set_binary(output::BinaryOutput *output);
  void set_speed(output::FloatOutput *output, float off_speed = 0.0, float low_speed = 0.33, float medium_speed = 0.66, float high_speed = 1.0);
  void set_oscillation(output::BinaryOutput *oscillating_output);

  FanState *get_state() const;
  void set_state(FanState *state);

  void setup() override;

  void loop() override;

 protected:
  FanState *state_{nullptr};
  output::BinaryOutput *binary_output_{nullptr};
  output::FloatOutput *speed_output_{nullptr};
  float off_speed_{};
  float low_speed_{};
  float medium_speed_{};
  float high_speed_{};
  output::BinaryOutput *oscillating_output_{nullptr};
  bool next_update_{true};
};

} // namespace fan

} // namespace esphomelib

#endif //ESPHOMELIB_FAN_BASIC_FAN_COMPONENT_H
