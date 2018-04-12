//
// Created by Otto Winter on 03.12.17.
//

#include "esphomelib/ota_component.h"
#include "esphomelib/log.h"
#include "esphomelib/application.h"
#include "esphomelib/esppreferences.h"
#include "esphomelib/defines.h"

#ifdef USE_OTA

#include <ArduinoOTA.h>

namespace esphomelib {

static const char *TAG = "ota";
#ifdef ARDUINO_ARCH_ESP32
  static const char *PREF_TAG = "ota"; ///< Tag for preferences.
  static const char *PREF_SAFE_MODE_COUNTER_KEY = "safe_mode";
#endif

void OTAComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up OTA...");
  ESP_LOGCONFIG(TAG, "    port: %u", this->port_);
  this->server_ = new WiFiServer(this->port_);
  this->server_->begin();

  if (!this->hostname_.empty()) {
    ESP_LOGCONFIG(TAG, "    hostname: '%s'", this->hostname_.c_str());
    ArduinoOTA.setHostname(this->hostname_.c_str());
  }
  ArduinoOTA.setPort(this->port_);
  switch (this->auth_type_) {
    case PLAINTEXT: {
      ArduinoOTA.setPassword(this->password_.c_str());
      break;
    }
    case HASH: {
      ArduinoOTA.setPasswordHash(this->password_.c_str());
      break;
    }
    case OPEN: {}
  }

  ArduinoOTA.onStart([this]() {
    ESP_LOGI(TAG, "OTA starting...");
    this->ota_triggered_ = true;
    this->at_ota_progress_message_ = 0;
  });
  ArduinoOTA.onEnd([&]() {
    ESP_LOGI(TAG, "OTA update finished!");
    ESP_LOGI(TAG, "Rebooting...");
    if (this->has_safe_mode_)
      // Don't make successful OTAs trigger boot loop detection.
      this->write_rtc_(0);
  });
  ArduinoOTA.onProgress([this](uint progress, uint total) {
    if (this->at_ota_progress_message_++ % 8 != 0)
      return; // only print every 8th message
    float percentage = float(progress) * 100 / float(total);
    ESP_LOGD(TAG, "OTA in progress: %0.1f%%", percentage);
  });
  ArduinoOTA.onError([this](ota_error_t error) {
    ESP_LOGE(TAG, "Error[%u]: ", error);
    switch (error) {
      case OTA_AUTH_ERROR: {
        ESP_LOGE(TAG, "  Auth Failed");
        break;
      }
      case OTA_BEGIN_ERROR: {
        ESP_LOGE(TAG, "  Begin Failed");
        break;
      }
      case OTA_CONNECT_ERROR: {
        ESP_LOGE(TAG, "  Connect Failed");
        break;
      }
      case OTA_RECEIVE_ERROR: {
        ESP_LOGE(TAG, "  Receive Failed");
        break;
      }
      case OTA_END_ERROR: {
        ESP_LOGE(TAG, "  End Failed");
        break;
      }
      default:ESP_LOGE(TAG, "  Unknown Error");
    }
    this->ota_triggered_ = false;
  });
  ArduinoOTA.begin();
}

void OTAComponent::loop() {
  do {
    ArduinoOTA.handle();
    yield();
  } while (this->ota_triggered_);

  if (this->has_safe_mode_ && (millis() - this->safe_mode_start_time_) > this->safe_mode_enable_time_) {
    this->has_safe_mode_ = false;
    // successful boot, reset counter
    ESP_LOGI(TAG, "Boot seems successful, resetting boot loop counter.");
    this->write_rtc_(0);
  }
}

OTAComponent::OTAComponent(uint16_t port, std::string hostname)
    : port_(port), hostname_(std::move(hostname)), auth_type_(OPEN), server_(nullptr) {

}

void OTAComponent::set_auth_open() {
  this->auth_type_ = OPEN;
}
void OTAComponent::set_auth_plaintext_password(const std::string &password) {
  this->auth_type_ = PLAINTEXT;
  this->password_ = password;
}
void OTAComponent::set_auth_password_hash(const std::string &hash) {
  this->auth_type_ = HASH;
  this->password_ = hash;
}
float OTAComponent::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT;
}
uint16_t OTAComponent::get_port() const {
  return this->port_;
}
void OTAComponent::set_port(uint16_t port) {
  this->port_ = port;
}
const std::string &OTAComponent::get_hostname() const {
  return this->hostname_;
}
void OTAComponent::set_hostname(const std::string &hostname) {
  this->hostname_ = sanitize_hostname(hostname);
}
void OTAComponent::start_safe_mode(uint8_t num_attempts, uint32_t enable_time) {
  this->has_safe_mode_ = true;
  this->safe_mode_start_time_ = millis();
  this->safe_mode_enable_time_ = enable_time;

  uint8_t rtc_data = this->read_rtc_();

  ESP_LOGCONFIG(TAG, "Safe mode enabled. There have been %u suspected unsuccessful boot attempts.", rtc_data);

  if (rtc_data >= num_attempts) {
    this->write_rtc_(0);

    ESP_LOGE(TAG, "Boot loop detected. Proceeding to safe mode.");
    assert(App.get_wifi() != nullptr);

    App.get_wifi()->setup_();
    this->setup_();

    ESP_LOGI(TAG, "Waiting for OTA attempt.");
    uint32_t begin = millis();
    while ((millis() - begin) < enable_time) {
      this->loop_();
      App.get_wifi()->loop_();
    }
    ESP_LOGE(TAG, "No OTA attempt made, restarting.");
    ESP.restart();
  } else {
    // increment counter
    this->write_rtc_(uint8_t(rtc_data + 1));
  }
}
void OTAComponent::write_rtc_(uint8_t val) {
#ifdef ARDUINO_ARCH_ESP8266
  uint32_t data = val;
  ESP.rtcUserMemoryWrite(0, &data, sizeof(data));
#endif
#ifdef ARDUINO_ARCH_ESP32
  global_preferences.put_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, static_cast<uint8_t>(val));
#endif
}
uint8_t OTAComponent::read_rtc_() {
#ifdef ARDUINO_ARCH_ESP8266
  uint32_t rtc_data;
  ESP.rtcUserMemoryRead(0, &rtc_data, sizeof(rtc_data));
  if (rtc_data > 255) // num attempts 255 at max
    return 0;
  return uint8_t(rtc_data);
#endif
#ifdef ARDUINO_ARCH_ESP32
  return global_preferences.get_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, 0);
#endif
}

#endif //USE_OTA

} // namespace esphomelib
