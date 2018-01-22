//
// Created by Otto Winter on 22.01.18.
//

#ifndef ESPHOMELIB_HAL_H
#define ESPHOMELIB_HAL_H

#ifdef ARDUINO_ARCH_ESP32
#include <esp32-hal.h>
#else
#include "Arduino.h"
#endif

#endif //ESPHOMELIB_HAL_H
