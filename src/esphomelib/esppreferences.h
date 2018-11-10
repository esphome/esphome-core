#ifndef ESPHOMELIB_ESPPREFERENCES_H
#define ESPHOMELIB_ESPPREFERENCES_H

#include <string>

#ifdef ARDUINO_ARCH_ESP32
  #include <Preferences.h>
#endif

#include "esphomelib/espmath.h"
#include "esphomelib/defines.h"

ESPHOMELIB_NAMESPACE_BEGIN

class ESPPreferenceObject {
 public:
  ESPPreferenceObject();
  ESPPreferenceObject(size_t rtc_offset, size_t length, uint32_t type);
  uint32_t &operator[] (int i);

  void save();

  template<typename T>
  void save(T *src);

  bool load();

  template<typename T>
  bool load(T *dest);

  size_t total_length_bytes() const;
  size_t total_length_uint() const;

  uint32_t *data() const;

  bool is_initialized() const;

 protected:
  void save_internal_();
  void load_internal_();

  uint32_t calculate_crc_() const;

  size_t rtc_offset_;
  size_t length_;
  uint32_t type_;
  uint32_t *data_;
};

class ESPPreferences {
 public:
  void begin(const std::string &name);
  ESPPreferenceObject make_preference(size_t length, uint32_t type);
  template<typename T>
  ESPPreferenceObject make_preference(uint32_t type);
 protected:
  friend ESPPreferenceObject;

  uint32_t current_offset_{100};
#ifdef ARDUINO_ARCH_ESP32
  Preferences preferences_;
#endif
};

extern ESPPreferences global_preferences;

template<typename T>
ESPPreferenceObject esphomelib::ESPPreferences::make_preference(uint32_t type) {
  return this->make_preference(sizeof(T), type);
}

template<typename T>
void ESPPreferenceObject::save(T *src) {
  memcpy(this->data(), src, sizeof(T));
  this->save();
}

template<typename T>
bool ESPPreferenceObject::load(T *dest) {
  if (!this->load())
    return false;

  memcpy(dest, this->data(), sizeof(T));
  return true;
}

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_ESPPREFERENCES_H
