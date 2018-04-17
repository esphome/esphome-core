//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/wifi_component.h"

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <esp_wifi.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#include <user_interface.h>
#endif

#include <utility>

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

static const char *TAG = "wifi";

float WiFiComponent::get_setup_priority() const {
  return setup_priority::WIFI;
}

void WiFiComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WiFi...");
  global_wifi_component = this;

#ifdef ARDUINO_ARCH_ESP32
  WiFi.onEvent(on_wifi_event);
#endif
  delay(10);

  WiFi.persistent(false);
  add_shutdown_hook([]() {
    WiFi.mode(WIFI_OFF);
  });

  if (this->has_sta() && this->has_ap()) {
    this->setup_ap_config();
    this->setup_sta_config();
  } else if (this->has_sta()) {
    this->setup_sta_config();
    this->wait_for_sta();
  } else if (this->has_ap()) {
    this->setup_ap_config();
  } else {
    assert(false);
  }
}

void WiFiComponent::loop() {
  if (this->has_sta() && this->has_ap()) {
    if (this->sta_on_ && WiFi.status() == WL_CONNECTED && this->ap_on_) {
      this->sta_connected();
      ESP_LOGI(TAG, "STA connected, disabling AP...");
      // Connected, disable AP
      WiFi.enableAP(false);
      this->ap_on_ = false;
    } else if (this->sta_on_ && WiFi.status() != WL_CONNECTED && !this->ap_on_) {
      ESP_LOGI(TAG, "STA disconnected, re-enabling AP...");
#ifdef ARDUINO_ARCH_ESP32
      // Fix Arduino WiFi WL_CONNECTED issue
      WiFi.enableSTA(false);
      this->sta_on_ = false;
#endif

      // Re-enable AP
      this->setup_ap_config();
    }
#ifdef ARDUINO_ARCH_ESP32
    else if (!this->sta_on_) {
      this->setup_sta_config(false);
    }
#endif
  } else if (this->has_sta()) {
    if (WiFi.status() != WL_CONNECTED) {
      ESP_LOGI(TAG, "Reconnecting WiFi...");
      this->setup_sta_config(false);
      this->wait_for_sta();
    }
  }
}

WiFiComponent::WiFiComponent() = default;

#ifdef ARDUINO_ARCH_ESP32
void WiFiComponent::on_wifi_event(WiFiEvent_t event) {
  const char *event_name;
  switch (event) {
    case SYSTEM_EVENT_WIFI_READY: event_name = "WiFi ready";
      break;
    case SYSTEM_EVENT_SCAN_DONE: event_name = "Scan done";
      break;
    case SYSTEM_EVENT_STA_START: event_name = "STA start";
      break;
    case SYSTEM_EVENT_STA_STOP: event_name = "STA stop";
      break;
    case SYSTEM_EVENT_STA_CONNECTED: event_name = "STA connected";
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED: event_name = "STA disconnected";
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: event_name = "STA auth mode change";
      break;
    case SYSTEM_EVENT_STA_GOT_IP: event_name = "STA got IP";
      break;
    case SYSTEM_EVENT_STA_LOST_IP: event_name = "STA lost IP";
      break;
    case SYSTEM_EVENT_AP_START: event_name = "AP start";
      break;
    case SYSTEM_EVENT_AP_STOP: event_name = "AP stop";
      break;
    case SYSTEM_EVENT_AP_STACONNECTED: event_name = "AP STA connected";
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED: event_name = "AP STA disconnected";
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED: event_name = "AP Probe Request received";
      break;
    default: event_name = "UNKNOWN";
      break;
  }

  if (event == SYSTEM_EVENT_STA_DISCONNECTED) {
    global_wifi_component->sta_on_ = false;
  }

  ESP_LOGV(TAG, "[WiFi event] %s (num=%d status=%u)", event_name, event, WiFi.status());
}
#endif

void WiFiComponent::set_sta_manual_ip(ManualIP manual_ip) {
  this->sta_manual_ip_ = std::move(manual_ip);
}
void WiFiComponent::set_ap_manual_ip(ManualIP manual_ip) {
  this->ap_manual_ip_ = std::move(manual_ip);
}
void WiFiComponent::wait_for_sta() {
  if (WiFi.status() == WL_CONNECTED)
    return;
  ESP_LOGI(TAG, "Waiting for WiFi connection");
  uint32_t start = millis();
  wl_status_t status;
  while ((status = WiFi.status()) != WL_CONNECTED) {
#ifdef ARDUINO_ARCH_ESP32
    if (!this->sta_on_)
      this->setup_sta_config(false);
#endif

    if (status == WL_CONNECT_FAILED || millis() - start > 30000) {
      ESP_LOGE(TAG, "    Can't connect to WiFi network");
      shutdown();
    }

    delay(250);
    ESP_LOGV(TAG, ". (status=%d)", status);
  }
  this->sta_connected();
}
bool WiFiComponent::has_ap() const {
  return !this->ap_ssid_.empty();
}
bool WiFiComponent::has_sta() const {
  return !this->sta_ssid_.empty();
}
void WiFiComponent::setup_sta_config(bool show_config) {
  if (show_config) {
    ESP_LOGCONFIG(TAG, "Setting up STA...");
  }
  bool ret = WiFi.enableSTA(true);
  if (!ret) {
    ESP_LOGE(TAG, "WiFi.enableSTA() failed");
  }
  WiFi.setAutoConnect(false);
#ifdef ARDUINO_ARCH_ESP32
  WiFi.setAutoReconnect(false);
#endif
#ifdef ARDUINO_ARCH_ESP8266
  WiFi.setAutoReconnect(true);
#endif

  if (show_config) {
    ESP_LOGCONFIG(TAG, "    STA SSID: '%s'", this->sta_ssid_.c_str());
    ESP_LOGCONFIG(TAG, "    STA Password: '%s'", this->sta_password_.c_str());
  }
  if (this->sta_manual_ip_) {
    if (show_config) {
      ESP_LOGCONFIG(TAG, "    STA Static IP: '%s'", this->sta_manual_ip_->static_ip.toString().c_str());
      ESP_LOGCONFIG(TAG, "    STA Gateway: '%s'", this->sta_manual_ip_->gateway.toString().c_str());
      ESP_LOGCONFIG(TAG, "    STA Subnet: '%s'", this->sta_manual_ip_->subnet.toString().c_str());
      if (!is_empty(this->sta_manual_ip_->dns1)) {
        ESP_LOGCONFIG(TAG, "    STA DNS1: '%s'", this->sta_manual_ip_->dns1.toString().c_str());
      }
      if (!is_empty(this->sta_manual_ip_->dns2)) {
        ESP_LOGCONFIG(TAG, "    STA DNS2: '%s'", this->sta_manual_ip_->dns2.toString().c_str());
      }
    }

    ret = WiFi.config(this->sta_manual_ip_->static_ip, this->sta_manual_ip_->gateway, this->sta_manual_ip_->subnet,
                      this->sta_manual_ip_->dns1, this->sta_manual_ip_->dns2);
    if (!ret)
      ESP_LOGE(TAG, "WiFi.config() failed!");
  }

  if (!this->hostname_.empty()) {
    if (show_config) {
      ESP_LOGCONFIG(TAG, "    STA Hostname: '%s'", this->hostname_.c_str());
    }
#ifdef ARDUINO_ARCH_ESP32
    ret = WiFi.setHostname(this->hostname_.c_str());
#endif
#ifdef ARDUINO_ARCH_ESP8266
    ret = WiFi.hostname(this->hostname_.c_str());
#endif
    if (!ret)
      ESP_LOGE(TAG, "WiFi.hostname() failed!");
  }

  wl_status_t reti = WiFi.begin(this->sta_ssid_.c_str(), this->sta_password_.c_str());
  if (reti == WL_CONNECT_FAILED) {
    ESP_LOGE(TAG, "WiFi.begin() failed: %d", reti);
  }
  this->sta_on_ = true;
}
void WiFiComponent::setup_ap_config() {
  ESP_LOGCONFIG(TAG, "Setting up AP...");
  bool ret = WiFi.enableAP(true);
  if (!ret) {
    ESP_LOGE(TAG, "WiFi.enableAP() failed");
  }

  ESP_LOGCONFIG(TAG, "    AP SSID: '%s'", this->ap_ssid_.c_str());
  ESP_LOGCONFIG(TAG, "    AP Password: '%s'", this->ap_password_.c_str());
  if (this->ap_manual_ip_) {
    ESP_LOGCONFIG(TAG, "    AP Static IP: '%s'", this->ap_manual_ip_->static_ip.toString().c_str());
    ESP_LOGCONFIG(TAG, "    AP Gateway: '%s'", this->ap_manual_ip_->gateway.toString().c_str());
    ESP_LOGCONFIG(TAG, "    AP Subnet: '%s'", this->ap_manual_ip_->subnet.toString().c_str());

    ret = WiFi.softAPConfig(this->ap_manual_ip_->static_ip, this->ap_manual_ip_->gateway,
                            this->ap_manual_ip_->subnet);
    if (!ret) {
      ESP_LOGE(TAG, "WiFi.softAPConfig() failed!");
    }
  }

#ifdef ARDUINO_ARCH_ESP32
  if (!this->hostname_.empty()) {
    ESP_LOGCONFIG(TAG, "    AP Hostname: '%s'", this->hostname_.c_str());
    ret = WiFi.softAPsetHostname(this->hostname_.c_str());
    if (!ret) {
      ESP_LOGE(TAG, "WiFi.softAPsetHostname() failed!");
    }
  }
#endif

  const char *passphrase = this->ap_password_.c_str();
  if (this->ap_password_.empty())
    passphrase = nullptr;
  ret = WiFi.softAP(this->ap_ssid_.c_str(), passphrase, this->ap_channel_);
  if (!ret) {
    ESP_LOGE(TAG, "WiFi.softAP() failed!");
  }

  ESP_LOGD(TAG, "WiFi AP set up.");
  ESP_LOGCONFIG(TAG, "    IP Address: %s", WiFi.softAPIP().toString().c_str());
  this->ap_on_ = true;
}

void WiFiComponent::sta_connected() {
  ESP_LOGI(TAG, "WiFi connected.");
  ESP_LOGCONFIG(TAG, "    IP Address: %s", WiFi.localIP().toString().c_str());
  ESP_LOGCONFIG(TAG, "    Subnet: %s", WiFi.subnetMask().toString().c_str());
  ESP_LOGCONFIG(TAG, "    Gateway: %s", WiFi.gatewayIP().toString().c_str());
  ESP_LOGCONFIG(TAG, "    DNS1: %s", WiFi.dnsIP(0).toString().c_str());
  ESP_LOGCONFIG(TAG, "    DNS2: %s", WiFi.dnsIP(1).toString().c_str());
}
void WiFiComponent::set_sta(const std::string &ssid, const std::string &password) {
  this->sta_ssid_ = ssid;
  this->sta_password_ = password;
}
void WiFiComponent::set_ap(const std::string &ssid, const std::string &password, uint8_t channel) {
  this->ap_ssid_ = ssid;
  this->ap_password_ = password;
  this->ap_channel_ = channel;
}
void WiFiComponent::set_hostname(std::string &&hostname) {
  this->hostname_ = std::move(hostname);
}
const std::string &WiFiComponent::get_hostname() {
  return this->hostname_;
}

WiFiComponent *global_wifi_component;

} // namespace esphomelib
