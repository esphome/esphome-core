#include "esphomelib/defines.h"

#ifdef USE_DC_MOTOR

#include "esphomelib/dc_motor/dc_motor.h"
#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace dc_motor {

static const char *TAG = "dc_motor";

void Dc_motor::set_direction(bool clockwise){
    this->direction_clockwise_ = clockwise;
}




} // namespace dc_motor

ESPHOMELIB_NAMESPACE_END

#endif //USE_DC_MOTOR
