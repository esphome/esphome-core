#ifndef ESPHOMELIB_DC_MOTOR_L298N_H
#define ESPHOMELIB_DC_MOTOR_L298N_H

#include "esphomelib/defines.h"

#ifdef USE_L298N

#include "esphomelib/component.h"
#include "esphomelib/dc_motor/dc_motor.h"
#include "esphomelib/esphal.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace dc_motor {

class L298n_half : public Dc_motor, public Component {
 public:
  L298n_half(GPIOPin *enable_pin_motor, GPIOPin *dir_pin_motor);
  void set_sleep_pin(const GPIOOutputPin &sleep_pin);
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  GPIOPin *enable_pin_;
  GPIOPin *dir_pin_;
  GPIOPin *sleep_pin_{nullptr};
};

} // namespace dc_motor

ESPHOMELIB_NAMESPACE_END

#endif //USE_L298N

#endif //ESPHOMELIB_DC_MOTOR_L298N_H
