#ifndef ESPHOMELIB_DC_MOTOR_DC_MOTOR_H
#define ESPHOMELIB_DC_MOTOR_DC_MOTOR_H

#include "esphomelib/defines.h"

#ifdef USE_DC_MOTOR

#include "esphomelib/component.h"
#include "esphomelib/automation.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace dc_motor {

#define LOG_STEPPER(this) \
    ESP_LOGCONFIG(TAG, "  has_angle_sensor", this->has_angle_sensor); 

class dc_motor {
 public:
  void move();
  void stop();
  void set_direction(bool clockwise);
  
 protected:
  bool has_angle_sensor = false;
};


} // namespace dc_motor

ESPHOMELIB_NAMESPACE_END

#endif //USE_DC_MOTOR

#endif //ESPHOMELIB_DC_MOTOR_DC_MOTOR_H
