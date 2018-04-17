//
// Created by Otto Winter on 22.01.18.
//

#include "esphomelib/log.h"

#include "esphomelib/log_component.h"

int esp_log_printf_(ESPLogLevel level, const char *tag, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  int ret = esp_log_vprintf_(level, tag, format, arg);
  va_end(arg);
  return ret;
}

int esp_log_vprintf_(ESPLogLevel level, const char *tag, const char *format, va_list args) {
  auto *log = esphomelib::global_log_component;
  if (log == nullptr)
    return 0;

  return log->log_vprintf_(level, tag, format, args);
}

int esp_idf_log_vprintf_(const char *format, va_list args) {
  return esp_log_vprintf_(ESPHOMELIB_LOG_LEVEL_INFO, "", format, args);
}
