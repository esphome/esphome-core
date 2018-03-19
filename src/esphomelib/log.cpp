//
// Created by Otto Winter on 22.01.18.
//

#include "esphomelib/log.h"
#include "esphomelib/log_component.h"

#ifdef ARDUINO_ARCH_ESP8266

// shamelessly copied from esp32/arduino :)
const char *pathToFileName(const char *path) {
  size_t i = 0;
  size_t pos = 0;
  auto *p = (char *) path;
  while (*p) {
    i++;
    if (*p == '/' || *p == '\\') {
      pos = i;
    }
    p++;
  }
  return path + pos;
}

int log_printf(const char *format, ...)  {
  va_list arg;
  va_start(arg, format);
  int ret = esphomelib::LogComponent::log_vprintf_(format, arg);
  va_end(arg);
  return ret;
}

#endif
