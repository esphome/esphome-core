#ifndef ESPHOME_FAN_BASIC_FAN_COMPONENT_H
#define ESPHOME_FAN_BASIC_FAN_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_FAN

#include "esphome/component.h"
#include "esphome/output/binary_output.h"
#include "esphome/output/float_output.h"
#include "esphome/fan/fan_state.h"

ESPHOME_NAMESPACE_BEGIN

namespace fan {

/// Simple fan helper that pushes the states to BinaryOutput/FloatOutput devices.
class BasicFanComponent : public Component {
 public:
  /** Create a fan that supports binary state operation (ON/OFF). Can't be mixed with set_speed.
   *
   * @param output The binary output where all binary commands should land.
   */
  void set_binary(output::BinaryOutput *output);

  /** Create a fan that supports speed operation (OFF/LOW/MEDIUM/HIGH SPEED). Can't be mixed with set_binary.
   *
   * @param output The FloatOutput where all speed/state commands should land.
   * @param low_speed The speed that should be sent to the output if the fan is in LOW speed mode.
   * @param medium_speed The speed that should be sent to the output if the fan is in MEDIUM speed mode.
   * @param high_speed The speed that should be sent to the output if the fan is in HIGH speed mode.
   */
  void set_speed(output::FloatOutput *output, float low_speed = 0.33, float medium_speed = 0.66,
                 float high_speed = 1.0);

  /** Set an oscillation output for this fan.
   *
   * @param oscillating_output The binary output where all oscillation commands should land.
   */
  void set_oscillation(output::BinaryOutput *oscillating_output);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  FanState *get_state() const;
  void set_state(FanState *state);
  void setup() override;
  void dump_config() override;
  void loop() override;

  float get_setup_priority() const override;

 protected:
  FanState *state_{nullptr};
  output::BinaryOutput *binary_output_{nullptr};
  output::FloatOutput *speed_output_{nullptr};
  float low_speed_{};
  float medium_speed_{};
  float high_speed_{};
  output::BinaryOutput *oscillating_output_{nullptr};
  bool next_update_{true};
};

}  // namespace fan

ESPHOME_NAMESPACE_END

#endif  // USE_FAN

#endif  // ESPHOME_FAN_BASIC_FAN_COMPONENT_H
