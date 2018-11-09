#include "esphomelib/esppreferences.h"

#include <functional>

#include "esphomelib/log.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "preferences";

ESPPreferenceObject::ESPPreferenceObject()
    : rtc_offset_(0), length_(0), type_(0), data_(nullptr) {

}
ESPPreferenceObject::ESPPreferenceObject(size_t rtc_offset, size_t length, uint32_t type)
    : rtc_offset_(rtc_offset), length_(length), type_(type) {
  this->length_ = ((this->length_ + 3) / 4) * 4;
  this->data_ = new uint32_t[this->total_length_uint()];
  for (uint32_t i = 0; i < this->total_length_uint(); i++)
    this->data_[i] = 0;
}
uint32_t &ESPPreferenceObject::operator[](int i) {
  if (!this->is_initialized())
    return this->type_;
  return this->data()[i];
}
bool ESPPreferenceObject::load() {
  if (!this->is_initialized()) {
    ESP_LOGV(TAG, "Load Pref Not initialized!");
    return false;
  }
  this->load_internal_();
  bool valid = this->data_[0] == this->type_ &&
      this->data_[this->total_length_uint() - 1] == this->calculate_crc_();

  ESP_LOGVV(TAG, "LOAD %u: valid=%d, 0=%u 1=%u 2=%u (Type=%u, CRC=%u)",
            this->rtc_offset_, valid ? 1:0, this->data_[0], this->data_[1], this->data_[2],
            this->type_, this->calculate_crc_());
  return valid;
}
void ESPPreferenceObject::save() {
  if (!this->is_initialized()) {
    ESP_LOGV(TAG, "Save Pref Not initialized!");
    return;
  }

  this->data_[0] = this->type_;
  this->data_[this->total_length_uint() - 1] = this->calculate_crc_();
  this->save_internal_();
  ESP_LOGVV(TAG, "SAVE %u: 0=%u 1=%u 2=%u (Type=%u, CRC=%u)",
            this->rtc_offset_, this->data_[0], this->data_[1], this->data_[2],
            this->type_, this->calculate_crc_());
}

#ifdef ARDUINO_ARCH_ESP8266
void ESPPreferenceObject::save_internal_() {
  ESP.rtcUserMemoryWrite(this->rtc_offset_, this->data_, this->total_length_bytes());
}
void ESPPreferenceObject::load_internal_() {
  ESP.rtcUserMemoryRead(this->rtc_offset_, this->data_, this->total_length_bytes());
}
void ESPPreferences::begin(const std::string &name) {

}
#endif

#ifdef ARDUINO_ARCH_ESP32
void ESPPreferenceObject::save_internal_() {
  char key[32];
  sprintf(key, "%u", this->rtc_offset_);
  size_t ret = global_preferences.preferences_.putBytes(key, this->data_, this->total_length_bytes());
  if (ret != this->total_length_bytes()) {
    ESP_LOGV(TAG, "putBytes failed!");
  }
}
void ESPPreferenceObject::load_internal_() {
  char key[32];
  sprintf(key, "%u", this->rtc_offset_);
  size_t ret = global_preferences.preferences_.getBytes(key, this->data_, this->total_length_bytes());
  if (ret != this->total_length_bytes()) {
    ESP_LOGV(TAG, "getBytes failed!");
  }
}
void ESPPreferences::begin(const std::string &name) {
  const std::string key = truncate_string(name, 15);
  ESP_LOGV(TAG, "Opening preferences with key '%s'", key.c_str());
  this->preferences_.begin(key.c_str());
}
#endif

size_t ESPPreferenceObject::total_length_bytes() const {
  // type + data + CRC
  return this->length_ + 2 * sizeof(uint32_t);
}
size_t ESPPreferenceObject::total_length_uint() const {
  return this->length_ / sizeof(uint32_t) + 2;
}
uint32_t *ESPPreferenceObject::data() const {
  return &this->data_[1];
}
uint32_t ESPPreferenceObject::calculate_crc_() const {
  uint32_t crc = 42;
  for (size_t i = 1; i < this->total_length_uint() - 1; i++) {
    crc ^= (this->data_[i] * 2654435769UL) >> 1;
  }
  return crc;
}
bool ESPPreferenceObject::is_initialized() const {
  return this->data_ != nullptr;
}

ESPPreferenceObject ESPPreferences::make_preference(size_t length, uint32_t type) {
  auto pref = ESPPreferenceObject(this->current_offset_, length, type);
  this->current_offset_ += pref.total_length_bytes();
  return pref;
}

ESPPreferences global_preferences;

ESPHOMELIB_NAMESPACE_END
