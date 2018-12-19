#include "esphomelib/log.h"

#include "esphomelib/log_component.h"

int HOT esp_log_printf_(int level, const char *tag, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  int ret = esp_log_vprintf_(level, tag, format, arg);
  va_end(arg);
  return ret;
}

int HOT esp_log_vprintf_(int level, const char *tag, const char *format, va_list args) {
  auto *log = esphomelib::global_log_component;
  if (log == nullptr)
    return 0;

  return log->log_vprintf_(level, tag, format, args);
}

int HOT esp_idf_log_vprintf_(const char *format, va_list args) {
  auto *log = esphomelib::global_log_component;
  if (log == nullptr)
    return 0;

  return log->log_vprintf_(log->get_global_log_level(), "", format, args);
}
