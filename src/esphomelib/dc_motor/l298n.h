#ifndef ESPHOMELIB_DC_MOTOR_L298N_H
#define ESPHOMELIB_DC_MOTOR_L298N_H

#include "esphomelib/defines.h"

#ifdef USE_DC_MOTOR_L298N

#include "esphomelib/component.h"
#include "esphomelib/dc_motor/de_motor.h"
#include "esphomelib/esphal.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace dc_motor {

class L298n : public Dc_motor, public Component {
 public:
  L298n(GPIOPin *enable_pin_motor_a, GPIOPin *dir_pin_motor_a,GPIOPin *enable_pin_motor_b, GPIOPin *dir_pin_motor_b);
  void set_sleep_pin(const GPIOOutputPin &sleep_pin);
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  GPIOPin *step_pin_;
  GPIOPin *dir_pin_;
  GPIOPin *sleep_pin_{nullptr};
  HighFrequencyLoopRequester high_freq_;
};

} // namespace dc_motor

ESPHOMELIB_NAMESPACE_END

#endif //USE_L298N

#endif //ESPHOMELIB_DC_MOTOR_L298N_H
