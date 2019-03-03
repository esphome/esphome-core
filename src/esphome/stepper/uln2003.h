#ifndef ESPHOME_STEPPER_ULN2003_H
#define ESPHOME_STEPPER_ULN2003_H

#include "esphome/defines.h"

#ifdef USE_ULN2003

#include "esphome/component.h"
#include "esphome/stepper/stepper.h"

ESPHOME_NAMESPACE_BEGIN

namespace stepper {

enum ULN2003StepMode {
  ULN2003_STEP_MODE_FULL_STEP,
  ULN2003_STEP_MODE_HALF_STEP,
  ULN2003_STEP_MODE_WAVE_DRIVE,
};

class ULN2003 : public Stepper, public Component {
 public:
  ULN2003(GPIOPin *a, GPIOPin *b, GPIOPin *c, GPIOPin *d);

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_sleep_when_done(bool sleep_when_done);
  void set_step_mode(ULN2003StepMode step_mode);

 protected:
  void write_step_(int32_t step);

  bool sleep_when_done_{false};
  GPIOPin *pin_a_;
  GPIOPin *pin_b_;
  GPIOPin *pin_c_;
  GPIOPin *pin_d_;
  ULN2003StepMode step_mode_{ULN2003_STEP_MODE_FULL_STEP};
  HighFrequencyLoopRequester high_freq_;
  int32_t current_uln_pos_{0};
};

}  // namespace stepper

ESPHOME_NAMESPACE_END

#endif  // USE_ULN2003

#endif  // ESPHOME_STEPPER_ULN2003_H
