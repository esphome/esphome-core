// Based on the awesome work of Jochen Krapf: https://github.com/jkDesignDE/MechInputs

#ifndef ESPHOME_ROTARY_ENCODER_H
#define ESPHOME_ROTARY_ENCODER_H

#include "esphome/defines.h"

#ifdef USE_ROTARY_ENCODER_SENSOR

#include "esphome/sensor/sensor.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

/// All possible resolutions for the rotary encoder
enum RotaryEncoderResolution {
  ROTARY_ENCODER_1_PULSE_PER_CYCLE = 0x11FF, /// increment counter by 1 with every A-B cycle, slow response but accurate
  ROTARY_ENCODER_2_PULSES_PER_CYCLE = 0x33FF, /// increment counter by 2 with every A-B cycle
  ROTARY_ENCODER_4_PULSES_PER_CYCLE = 0x77FF, /// increment counter by 4 with every A-B cycle, most inaccurate
};

class RotaryEncoderSensor : public Sensor, public Component {
 public:
  RotaryEncoderSensor(const std::string &name, GPIOPin *pin_a, GPIOPin *pin_b);

  /** Set the resolution of the rotary encoder.
   *
   * By default, this component will increment the counter by 1 with every A-B input cycle.
   * You can however change this behavior to have more coarse resolutions like 4 counter increases per A-B cycle.
   *
   * @param mode The new mode of the encoder.
   */
  void set_resolution(RotaryEncoderResolution mode);

  void set_reset_pin(const GPIOInputPin &pin_i);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  void dump_config() override;
  void loop() override;
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

  float get_setup_priority() const override;

 protected:
  /// The ISR that handles pushing all interrupts to process_state_machine_ of all rotary encoders.
  static void encoder_isr_();

  /// Process the state machine state of this rotary encoder. Called from encoder_isr_
  void process_state_machine_();

  GPIOPin *pin_a_;
  GPIOPin *pin_b_;
  GPIOPin *pin_i_{nullptr}; /// Index pin, if this is not nullptr, the counter will reset to 0 once this pin is HIGH.

  volatile int32_t counter_{0}; /// The internal counter for steps
  volatile bool has_changed_{true};
  uint16_t state_{0};
  RotaryEncoderResolution resolution_{ROTARY_ENCODER_1_PULSE_PER_CYCLE};
};

/// Global storage for having multiple rotary encoders with a single ISR
extern std::vector<RotaryEncoderSensor *> global_rotary_encoders_;

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_ROTARY_ENCODER_SENSOR

#endif //ESPHOME_ROTARY_ENCODER_H
