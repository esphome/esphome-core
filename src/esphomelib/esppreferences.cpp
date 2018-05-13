//
// Created by Otto Winter on 27.01.18.
//

#include "esphomelib/esppreferences.h"

#include <functional>

#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

#ifdef ARDUINO_ARCH_ESP32
void ESPPreferences::begin(const std::string &name) {
  this->preferences_.begin(truncate_string(name, 15).c_str());
}
std::string ESPPreferences::get_preference_key(const std::string &friendly_name, const std::string &key) {
  // TODO: Improve this - the hash function is less than ideal.
  size_t h = std::hash<std::string>{}(friendly_name);
  char buffer[8];
  snprintf(buffer, sizeof(buffer), "%0zx", h);
  std::string name_hash = std::string(buffer);
  std::string trunc_key = truncate_string(key, 7);
  return name_hash + "-" + trunc_key;
}
bool ESPPreferences::get_bool(const std::string &friendly_name, const std::string &key, bool default_value) {
  return this->preferences_.getBool(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
int8_t ESPPreferences::get_int8(const std::string &friendly_name, const std::string &key, int8_t default_value) {
  return this->preferences_.getChar(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
uint8_t ESPPreferences::get_uint8(const std::string &friendly_name, const std::string &key, uint8_t default_value) {
  return this->preferences_.getUChar(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
int16_t ESPPreferences::get_int16(const std::string &friendly_name, const std::string &key, int16_t default_value) {
  return this->preferences_.getShort(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
uint16_t ESPPreferences::get_uint16(const std::string &friendly_name, const std::string &key, uint16_t default_value) {
  return this->preferences_.getUShort(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
int32_t ESPPreferences::get_int32(const std::string &friendly_name, const std::string &key, int32_t default_value) {
  return this->preferences_.getInt(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
uint32_t ESPPreferences::get_uint32(const std::string &friendly_name, const std::string &key, uint32_t default_value) {
  return this->preferences_.getUInt(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
int64_t ESPPreferences::get_int64(const std::string &friendly_name, const std::string &key, int64_t default_value) {
  return this->preferences_.getLong64(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
uint64_t ESPPreferences::get_uint64(const std::string &friendly_name, const std::string &key, uint64_t default_value) {
  return this->preferences_.getULong64(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
float ESPPreferences::get_float(const std::string &friendly_name, const std::string &key, float default_value) {
  return this->preferences_.getFloat(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
double ESPPreferences::get_double(const std::string &friendly_name, const std::string &key, double default_value) {
  return this->preferences_.getDouble(this->get_preference_key(friendly_name, key).c_str(), default_value);
}
size_t ESPPreferences::put_bool(const std::string &friendly_name, const std::string &key, bool value) {
  return this->preferences_.putBool(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_int8(const std::string &friendly_name, const std::string &key, int8_t value) {
  return this->preferences_.putChar(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_uint8(const std::string &friendly_name, const std::string &key, uint8_t value) {
  return this->preferences_.putUChar(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_int16(const std::string &friendly_name, const std::string &key, int16_t value) {
  return this->preferences_.putShort(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_uint16(const std::string &friendly_name, const std::string &key, uint16_t value) {
  return this->preferences_.putUShort(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_int32(const std::string &friendly_name, const std::string &key, int32_t value) {
  return this->preferences_.putInt(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_uint32(const std::string &friendly_name, const std::string &key, uint32_t value) {
  return this->preferences_.putUInt(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_int64(const std::string &friendly_name, const std::string &key, int64_t value) {
  return this->preferences_.putLong64(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_uint64(const std::string &friendly_name, const std::string &key, uint64_t value) {
  return this->preferences_.putULong64(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_float(const std::string &friendly_name, const std::string &key, float value) {
  return this->preferences_.putFloat(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_double(const std::string &friendly_name, const std::string &key, double value) {
  return this->preferences_.putDouble(this->get_preference_key(friendly_name, key).c_str(), value);
}
#endif
#ifdef ARDUINO_ARCH_ESP8266
// TODO implement this for ESP8266

void ESPPreferences::begin(const std::string &name) {

}
bool ESPPreferences::get_bool(const std::string &friendly_name, const std::string &key, bool default_value) {
  return default_value;
}
int8_t ESPPreferences::get_int8(const std::string &friendly_name, const std::string &key, int8_t default_value) {
  return default_value;
}
uint8_t ESPPreferences::get_uint8(const std::string &friendly_name, const std::string &key, uint8_t default_value) {
  return default_value;
}
int16_t ESPPreferences::get_int16(const std::string &friendly_name, const std::string &key, int16_t default_value) {
  return default_value;
}
uint16_t ESPPreferences::get_uint16(const std::string &friendly_name, const std::string &key, uint16_t default_value) {
  return default_value;
}
int32_t ESPPreferences::get_int32(const std::string &friendly_name, const std::string &key, int32_t default_value) {
  return default_value;
}
uint32_t ESPPreferences::get_uint32(const std::string &friendly_name, const std::string &key, uint32_t default_value) {
  return default_value;
}
int64_t ESPPreferences::get_int64(const std::string &friendly_name, const std::string &key, int64_t default_value) {
  return default_value;
}
uint64_t ESPPreferences::get_uint64(const std::string &friendly_name, const std::string &key, uint64_t default_value) {
  return default_value;
}
float ESPPreferences::get_float(const std::string &friendly_name, const std::string &key, float default_value) {
  return default_value;
}
double ESPPreferences::get_double(const std::string &friendly_name, const std::string &key, double default_value) {
  return default_value;
}
size_t ESPPreferences::put_bool(const std::string &friendly_name, const std::string &key, bool value) {
  return 0;
}
size_t ESPPreferences::put_int8(const std::string &friendly_name, const std::string &key, int8_t value) {
  return 0;
}
size_t ESPPreferences::put_uint8(const std::string &friendly_name, const std::string &key, uint8_t value) {
  return 0;
}
size_t ESPPreferences::put_int16(const std::string &friendly_name, const std::string &key, int16_t value) {
  return 0;
}
size_t ESPPreferences::put_uint16(const std::string &friendly_name, const std::string &key, uint16_t value) {
  return 0;
}
size_t ESPPreferences::put_int32(const std::string &friendly_name, const std::string &key, int32_t value) {
  return 0;
}
size_t ESPPreferences::put_uint32(const std::string &friendly_name, const std::string &key, uint32_t value) {
  return 0;
}
size_t ESPPreferences::put_int64(const std::string &friendly_name, const std::string &key, int64_t value) {
  return 0;
}
size_t ESPPreferences::put_uint64(const std::string &friendly_name, const std::string &key, uint64_t value) {
  return 0;
}
size_t ESPPreferences::put_float(const std::string &friendly_name, const std::string &key, float value) {
  return 0;
}
size_t ESPPreferences::put_double(const std::string &friendly_name, const std::string &key, double value) {
  return 0;
}
#endif

ESPPreferences global_preferences;

ESPHOMELIB_NAMESPACE_END
