//
// Created by Otto Winter on 22.01.18.
//

#ifndef ESPHOMELIB_ESPHAL_H
#define ESPHOMELIB_ESPHAL_H

#ifdef ARDUINO_ARCH_ESP32
#include <esp32-hal.h>
#else
#include "Arduino.h"
#include "esphomelib/espmath.h"
#endif

#endif //ESPHOMELIB_ESPHAL_H
