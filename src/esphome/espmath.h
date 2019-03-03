#ifndef ESPHOME_ESPMATH_H
#define ESPHOME_ESPMATH_H

#include "Arduino.h"

#ifdef round
#undef round
#endif

#ifdef abs
#undef abs
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include "math.h"  // NOLINT

#endif  // ESPHOME_ESPMATH_H
