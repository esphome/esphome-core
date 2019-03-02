#include "esphome/log.h"

#include "esphome/log_component.h"

int HOT esp_log_printf_(int level, const char *tag, const char *format, ...) {  // NOLINT
  va_list arg;
  va_start(arg, format);
  int ret = esp_log_vprintf_(level, tag, format, arg);
  va_end(arg);
  return ret;
}
#ifdef USE_STORE_LOG_STR_IN_FLASH
int HOT esp_log_printf_(int level, const char *tag, const __FlashStringHelper *format, ...) {
  va_list arg;
  va_start(arg, format);
  int ret = esp_log_vprintf_(level, tag, format, arg);
  va_end(arg);
  return ret;
}
#endif

int HOT esp_log_vprintf_(int level, const char *tag, const char *format, va_list args) {  // NOLINT
  auto *log = esphome::global_log_component;
  if (log == nullptr)
    return 0;

  return log->log_vprintf_(level, tag, format, args);
}

#ifdef USE_STORE_LOG_STR_IN_FLASH
int HOT esp_log_vprintf_(int level, const char *tag, const __FlashStringHelper *format, va_list args) {  // NOLINT
  auto *log = esphome::global_log_component;
  if (log == nullptr)
    return 0;

  return log->log_vprintf_(level, tag, format, args);
}
#endif

int HOT esp_idf_log_vprintf_(const char *format, va_list args) {  // NOLINT
  auto *log = esphome::global_log_component;
  if (log == nullptr)
    return 0;

  return log->log_vprintf_(log->get_global_log_level(), "", format, args);
}
