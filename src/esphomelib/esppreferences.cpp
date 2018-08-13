//
// Created by Otto Winter on 27.01.18.
//

#include "esphomelib/esppreferences.h"

#include <functional>

#include "esphomelib/log.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

#ifdef ESPHOMELIB_LOG_HAS_VERBOSE
static const char *TAG = "preferences";
#endif

#ifdef ARDUINO_ARCH_ESP32
void ESPPreferences::begin(const std::string &name) {
  const std::string key = truncate_string(name, 15);
  ESP_LOGV(TAG, "Opening preferences with key '%s'", key.c_str());
  this->preferences_.begin(key.c_str());
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
  const std::string key_ = this->get_preference_key(friendly_name, key);
  bool value = this->preferences_.getBool(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered bool %s", key_.c_str(), value ? "ON" : "OFF");
  return value;
}
int8_t ESPPreferences::get_int8(const std::string &friendly_name, const std::string &key, int8_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  int8_t value = this->preferences_.getChar(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered int8 %d", key_.c_str(), value);
  return value;
}
uint8_t ESPPreferences::get_uint8(const std::string &friendly_name, const std::string &key, uint8_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  uint8_t value = this->preferences_.getUChar(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered uint8 %u", key_.c_str(), value);
  return value;
}
int16_t ESPPreferences::get_int16(const std::string &friendly_name, const std::string &key, int16_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  int16_t value = this->preferences_.getShort(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered int16 %d", key_.c_str(), value);
  return value;
}
uint16_t ESPPreferences::get_uint16(const std::string &friendly_name, const std::string &key, uint16_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  uint16_t value = this->preferences_.getUShort(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered uint16 %u", key_.c_str(), value);
  return value;
}
int32_t ESPPreferences::get_int32(const std::string &friendly_name, const std::string &key, int32_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  int32_t value = this->preferences_.getInt(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered int32 %d", key_.c_str(), value);
  return value;
}
uint32_t ESPPreferences::get_uint32(const std::string &friendly_name, const std::string &key, uint32_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  uint32_t value = this->preferences_.getUInt(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered uint32 %u", key_.c_str(), value);
  return value;
}
int64_t ESPPreferences::get_int64(const std::string &friendly_name, const std::string &key, int64_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  int64_t value = this->preferences_.getLong64(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered int64 %lld", key_.c_str(), value);
  return value;
}
uint64_t ESPPreferences::get_uint64(const std::string &friendly_name, const std::string &key, uint64_t default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  uint64_t value = this->preferences_.getULong64(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered uint64 %llu", key_.c_str(), value);
  return value;
}
float ESPPreferences::get_float(const std::string &friendly_name, const std::string &key, float default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  float value = this->preferences_.getFloat(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered float %f", key_.c_str(), value);
  return value;
}
double ESPPreferences::get_double(const std::string &friendly_name, const std::string &key, double default_value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  double value = this->preferences_.getFloat(key_.c_str(), default_value);
  ESP_LOGVV(TAG, "'%s' -> recovered double %llf", key_.c_str(), value);
  return value;
}
size_t ESPPreferences::put_bool(const std::string &friendly_name, const std::string &key, bool value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting bool %s", key_.c_str(), value ? "ON" : "OFF");
  return this->preferences_.putBool(key_.c_str(), value);
}
size_t ESPPreferences::put_int8(const std::string &friendly_name, const std::string &key, int8_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting int8 %d", key_.c_str(), value);
  return this->preferences_.putChar(key_.c_str(), value);
}
size_t ESPPreferences::put_uint8(const std::string &friendly_name, const std::string &key, uint8_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting uint8 %u", key_.c_str(), value);
  return this->preferences_.putUChar(key_.c_str(), value);
  return this->preferences_.putUChar(this->get_preference_key(friendly_name, key).c_str(), value);
}
size_t ESPPreferences::put_int16(const std::string &friendly_name, const std::string &key, int16_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting int16 %d", key_.c_str(), value);
  return this->preferences_.putShort(key_.c_str(), value);
}
size_t ESPPreferences::put_uint16(const std::string &friendly_name, const std::string &key, uint16_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting uint16 %u", key_.c_str(), value);
  return this->preferences_.putUShort(key_.c_str(), value);
}
size_t ESPPreferences::put_int32(const std::string &friendly_name, const std::string &key, int32_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting int32 %d", key_.c_str(), value);
  return this->preferences_.putInt(key_.c_str(), value);
}
size_t ESPPreferences::put_uint32(const std::string &friendly_name, const std::string &key, uint32_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting uint32 %u", key_.c_str(), value);
  return this->preferences_.putUInt(key_.c_str(), value);
}
size_t ESPPreferences::put_int64(const std::string &friendly_name, const std::string &key, int64_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting int64 %lld", key_.c_str(), value);
  return this->preferences_.putLong64(key_.c_str(), value);
}
size_t ESPPreferences::put_uint64(const std::string &friendly_name, const std::string &key, uint64_t value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting uint64 %llu", key_.c_str(), value);
  return this->preferences_.putULong64(key_.c_str(), value);
}
size_t ESPPreferences::put_float(const std::string &friendly_name, const std::string &key, float value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting float %f", key_.c_str(), value);
  return this->preferences_.putFloat(key_.c_str(), value);
}
size_t ESPPreferences::put_double(const std::string &friendly_name, const std::string &key, double value) {
  const std::string key_ = this->get_preference_key(friendly_name, key);
  ESP_LOGVV(TAG, "'%s' -> putting double %f", key_.c_str(), value);
  return this->preferences_.putDouble(key_.c_str(), value);
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
