#ifndef ESPHOMELIB_DC_MOTOR_H_BRIDGE_H
#define ESPHOMELIB_DC_MOTOR_H_BRIDGE_H

#include "esphomelib/defines.h"

#ifdef USE_H_BRIDGE

#include "esphomelib/component.h"
#include "esphomelib/dc_motor/dc_motor.h"
#include "esphomelib/esphal.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace dc_motor {

class H_bridge : public Dc_motor, public Component {
 public:
  H_bridge(GPIOPin *positive_pin, GPIOPin *nagative_pin);
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  GPIOPin *positive_pin_;
  GPIOPin *negative_pin_;
};


} // namespace dc_motor

ESPHOMELIB_NAMESPACE_END

#endif //USE_YX75V18

#endif //ESPHOMELIB_DC_MOTOR_YX75V18_H
