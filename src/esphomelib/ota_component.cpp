//
// Created by Otto Winter on 03.12.17.
//

#include <esphomelib/log.h>
#include <ArduinoOTA.h>

#include "ota_component.h"

namespace esphomelib {

static const char *TAG = "ota";

void OTAComponent::setup() {
  ESP_LOGD(TAG, "Setting up OTA...");
  this->server_ = new WiFiServer(this->port_);
  this->server_->begin();

  if (!this->hostname_.empty())
    ArduinoOTA.setHostname(this->hostname_.c_str());
  ArduinoOTA.setPort(this->port_);
  switch (this->auth_type_) {
    case PLAINTEXT: {
      ArduinoOTA.setPassword(this->password_.c_str());
      break;
    }
    case HASH: {
      ArduinoOTA.setPasswordHash(this->password_hash_.c_str());
      break;
    }
    case OPEN: {}
  }

  ArduinoOTA.onStart([]() { ESP_LOGI(TAG, "OTA starting..."); });
  ArduinoOTA.onEnd([]() {
    ESP_LOGI(TAG, "OTA update finished!");
    ESP_LOGI(TAG, "Rebooting...");
  });
  ArduinoOTA.onProgress([](uint progress, uint total) {
    float percentage = float(progress) * 100 / float(total);
    ESP_LOGI(TAG, "OTA in progress: %0.1f%%", percentage);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    ESP_LOGE(TAG, "Error[%u]: ", error);
    switch (error) {
      case OTA_AUTH_ERROR: {
        ESP_LOGE(TAG, "Auth Failed");
        break;
      }
      case OTA_BEGIN_ERROR: {
        ESP_LOGE(TAG, "Begin Failed");
        break;
      }
      case OTA_CONNECT_ERROR: {
        ESP_LOGE(TAG, "Connect Failed");
        break;
      }
      case OTA_RECEIVE_ERROR: {
        ESP_LOGE(TAG, "Receive Failed");
        break;
      }
      case OTA_END_ERROR: {
        ESP_LOGE(TAG, "End Failed");
        break;
      }
      default:ESP_LOGE(TAG, "Unknown Error");
    }
  });
  ArduinoOTA.begin();
}

void OTAComponent::loop() {
  ArduinoOTA.handle();
}

OTAComponent::OTAComponent(uint16_t port, std::string hostname)
    : port_(port), hostname_(std::move(hostname)), auth_type_(OPEN), server_(nullptr) {}

void OTAComponent::set_auth_open() {
  this->auth_type_ = OPEN;
}
void OTAComponent::set_auth_plaintext_password(const std::string &password) {
  this->auth_type_ = PLAINTEXT;
  this->password_ = password;
}
void OTAComponent::set_auth_password_hash(const std::string &hash) {
  this->auth_type_ = HASH;
  this->password_hash_ = hash;
}
float OTAComponent::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT;
}
uint16_t OTAComponent::get_port() const {
  return port_;
}
void OTAComponent::set_port(uint16_t port) {
  port_ = port;
}
const std::string &OTAComponent::get_hostname() const {
  return hostname_;
}
void OTAComponent::set_hostname(const std::string &hostname) {
  hostname_ = hostname;
}

} // namespace esphomelib
