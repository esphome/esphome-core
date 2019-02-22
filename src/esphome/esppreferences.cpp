#include "esphome/esppreferences.h"

#include <functional>

#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

static const char *TAG = "preferences";

ESPPreferenceObject::ESPPreferenceObject()
    : rtc_offset_(0), length_words_(0), type_(0), data_(nullptr) {

}
ESPPreferenceObject::ESPPreferenceObject(size_t rtc_offset, size_t length, uint32_t type)
    : rtc_offset_(rtc_offset), length_words_(length), type_(type) {
  this->data_ = new uint32_t[this->length_words_ + 1];
  for (uint32_t i = 0; i < this->length_words_ + 1; i++)
    this->data_[i] = 0;
}
bool ESPPreferenceObject::load_() {
  if (!this->is_initialized()) {
    ESP_LOGV(TAG, "Load Pref Not initialized!");
    return false;
  }
  if (!this->load_internal_())
    return false;

  bool valid = this->data_[this->length_words_] == this->calculate_crc_();

  ESP_LOGVV(TAG, "LOAD %u: valid=%s, 0=%u 1=%u (Type=%u, CRC=%u)",
            this->rtc_offset_, YESNO(valid), this->data_[0], this->data_[1],
            this->type_, this->calculate_crc_());
  return valid;
}
bool ESPPreferenceObject::save_() {
  if (!this->is_initialized()) {
    ESP_LOGV(TAG, "Save Pref Not initialized!");
    return false;
  }

  this->data_[this->length_words_] = this->calculate_crc_();
  if (!this->save_internal_())
    return false;
  ESP_LOGVV(TAG, "SAVE %u: 0=%u 1=%u (Type=%u, CRC=%u)",
            this->rtc_offset_, this->data_[0], this->data_[1],
            this->type_, this->calculate_crc_());
  return true;
}

#ifdef ARDUINO_ARCH_ESP8266

#define ESP_RTC_USER_MEM_START 0x60001200
#define ESP_RTC_USER_MEM ((uint32_t *) ESP_RTC_USER_MEM_START)
#define ESP_RTC_USER_MEM_SIZE_WORDS 128

static inline bool esp_rtc_user_mem_read(uint32_t index, uint32_t *dest) {
  if (index >= ESP_RTC_USER_MEM_SIZE_WORDS) {
    return false;
  }
  *dest = ESP_RTC_USER_MEM[index];
  return true;
}

static inline bool esp_rtc_user_mem_write(uint32_t index, uint32_t value) {
  if (index >= ESP_RTC_USER_MEM_SIZE_WORDS) {
    return false;
  }
  if (index < 32 && global_preferences.is_prevent_write()) {
    return false;
  }

  ESP_RTC_USER_MEM[index] = value;
  return true;
}

bool ESPPreferenceObject::save_internal_() {
  for (uint32_t i = 0; i <= this->length_words_; i++) {
    if (!esp_rtc_user_mem_write(this->rtc_offset_ + i, this->data_[i]))
      return false;
  }
  return true;
}
bool ESPPreferenceObject::load_internal_() {
  for (uint32_t i = 0; i <= this->length_words_; i++) {
    if (!esp_rtc_user_mem_read(this->rtc_offset_ + i, &this->data_[i]))
      return false;
  }
  return true;
}
ESPPreferences::ESPPreferences()
  // offset starts from start of user RTC mem (64 words before that are reserved for system),
  // an additional 32 words at the start of user RTC are for eboot (OTA, see eboot_command.h),
  // which will be reset each time OTA occurs
    : current_offset_(0) {

}

void ESPPreferences::begin(const std::string &name) {

}

ESPPreferenceObject ESPPreferences::make_preference(size_t length, uint32_t type) {
  uint32_t start = this->current_offset_;
  uint32_t end = start + length + 1;
  bool in_normal = start < 96;
  // Normal: offset 0-95 maps to RTC offset 32 - 127,
  // Eboot: offset 96-127 maps to RTC offset 0 - 31 words
  if (in_normal && end > 96) {
    // start is in normal but end is not -> switch to Eboot
    this->current_offset_ = start = 96;
    end = start + length + 1;
    in_normal = false;
  }

  if (end > 128) {
    // Doesn't fit in data, return uninitialized preference obj.
    return ESPPreferenceObject();
  }

  uint32_t rtc_offset;
  if (in_normal) {
    rtc_offset = start + 32;
  } else {
    rtc_offset = start - 96;
  }

  auto pref = ESPPreferenceObject(rtc_offset, length, type);
  this->current_offset_ += length + 1;
  return pref;
}
void ESPPreferences::prevent_write(bool prevent) {
  this->prevent_write_ = prevent;
}
bool ESPPreferences::is_prevent_write() {
  return this->prevent_write_;
}
#endif

#ifdef ARDUINO_ARCH_ESP32
bool ESPPreferenceObject::save_internal_() {
  char key[32];
  sprintf(key, "%u", this->rtc_offset_);
  uint32_t len = (this->length_words_ + 1) * 4;
  size_t ret = global_preferences.preferences_.putBytes(key, this->data_, len);
  if (ret != len) {
    ESP_LOGV(TAG, "putBytes failed!");
    return false;
  }
  return true;
}
bool ESPPreferenceObject::load_internal_() {
  char key[32];
  sprintf(key, "%u", this->rtc_offset_);
  uint32_t len = (this->length_words_ + 1) * 4;
  size_t ret = global_preferences.preferences_.getBytes(key, this->data_, len);
  if (ret != len) {
    ESP_LOGV(TAG, "getBytes failed!");
    return false;
  }
  return true;
}
ESPPreferences::ESPPreferences()
    : current_offset_(0) {

}
void ESPPreferences::begin(const std::string &name) {
  const std::string key = truncate_string(name, 15);
  ESP_LOGV(TAG, "Opening preferences with key '%s'", key.c_str());
  this->preferences_.begin(key.c_str());
}

ESPPreferenceObject ESPPreferences::make_preference(size_t length, uint32_t type) {
  auto pref = ESPPreferenceObject(this->current_offset_, length, type);
  this->current_offset_++;
  return pref;
}
#endif
uint32_t ESPPreferenceObject::calculate_crc_() const {
  uint32_t crc = this->type_;
  for (size_t i = 0; i < this->length_words_; i++) {
    crc ^= (this->data_[i] * 2654435769UL) >> 1;
  }
  return crc;
}
bool ESPPreferenceObject::is_initialized() const {
  return this->data_ != nullptr;
}

ESPPreferences global_preferences;

ESPHOME_NAMESPACE_END
