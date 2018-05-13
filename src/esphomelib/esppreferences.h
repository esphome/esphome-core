//
// Created by Otto Winter on 27.01.18.
//

#ifndef ESPHOMELIB_ESPPREFERENCES_H
#define ESPHOMELIB_ESPPREFERENCES_H

#include <string>

#ifdef ARDUINO_ARCH_ESP32
#include <Preferences.h>
#endif

#include "esphomelib/espmath.h"
#include "esphomelib/defines.h"

ESPHOMELIB_NAMESPACE_BEGIN

/// Helper class to allow easy access to non-volatile storage like SPIFFS to save preferences.
class ESPPreferences {
 public:
  /// Start the preferences object with the specified app name.
  void begin(const std::string &name);

  size_t put_bool(const std::string &friendly_name, const std::string &key, bool value);
  size_t put_int8(const std::string &friendly_name, const std::string &key, int8_t value);
  size_t put_uint8(const std::string &friendly_name, const std::string &key, uint8_t value);
  size_t put_int16(const std::string &friendly_name, const std::string &key, int16_t value);
  size_t put_uint16(const std::string &friendly_name, const std::string &key, uint16_t value);
  size_t put_int32(const std::string &friendly_name, const std::string &key, int32_t value);
  size_t put_uint32(const std::string &friendly_name, const std::string &key, uint32_t value);
  size_t put_int64(const std::string &friendly_name, const std::string &key, int64_t value);
  size_t put_uint64(const std::string &friendly_name, const std::string &key, uint64_t value);
  size_t put_float(const std::string &friendly_name, const std::string &key, float value);
  size_t put_double(const std::string &friendly_name, const std::string &key, double value);

  bool get_bool(const std::string &friendly_name, const std::string &key, bool default_value);
  int8_t get_int8(const std::string &friendly_name, const std::string &key, int8_t default_value);
  uint8_t get_uint8(const std::string &friendly_name, const std::string &key, uint8_t default_value);
  int16_t get_int16(const std::string &friendly_name, const std::string &key, int16_t default_value);
  uint16_t get_uint16(const std::string &friendly_name, const std::string &key, uint16_t default_value);
  int32_t get_int32(const std::string &friendly_name, const std::string &key, int32_t default_value);
  uint32_t get_uint32(const std::string &friendly_name, const std::string &key, uint32_t default_value);
  int64_t get_int64(const std::string &friendly_name, const std::string &key, int64_t default_value);
  uint64_t get_uint64(const std::string &friendly_name, const std::string &key, uint64_t default_value);
  float get_float(const std::string &friendly_name, const std::string &key, float default_value);
  double get_double(const std::string &friendly_name, const std::string &key, double default_value);

 protected:

#ifdef ARDUINO_ARCH_ESP32
  /// Return a key for the nvs storage by hashing the friendly name and truncating the key to 7 characters.
  std::string get_preference_key(const std::string &friendly_name, const std::string &key);

  Preferences preferences_;
#endif
};

extern ESPPreferences global_preferences;

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_ESPPREFERENCES_H
