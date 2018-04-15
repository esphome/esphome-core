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
#ifdef ARDUINO_ARCH_ESP32
  WiFi.onEvent(on_wifi_event);
#endif
  delay(10);
  ESP_LOGCONFIG(TAG, "    SSID: '%s'", this->ssid_.c_str());
  ESP_LOGCONFIG(TAG, "    Password: '%s'", this->password_.c_str());

  WiFi.persistent(false);
  bool ret = WiFi.mode(WIFI_STA);
  if (!ret)
    ESP_LOGE(TAG, "WiFi.mode(WIFI_STA) failed!");
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);

  if (!this->hostname_.empty()) {
    ESP_LOGCONFIG(TAG, "    Hostname: '%s'", this->hostname_.c_str());
#ifdef ARDUINO_ARCH_ESP32
    WiFi.setHostname(this->hostname_.c_str());
#endif
#ifdef ARDUINO_ARCH_ESP8266
    ret = WiFi.hostname(this->hostname_.c_str());
    if (!ret)
      ESP_LOGE(TAG, "WiFi.hostname() failed!");
#endif
  }

  if (this->manual_ip_) {
    ESP_LOGCONFIG(TAG, "    Static IP: '%s'", this->manual_ip_->static_ip.toString().c_str());
    ESP_LOGCONFIG(TAG, "    Gateway: '%s'", this->manual_ip_->gateway.toString().c_str());
    ESP_LOGCONFIG(TAG, "    Subnet: '%s'", this->manual_ip_->subnet.toString().c_str());
    if (!is_empty(this->manual_ip_->dns1))
      ESP_LOGCONFIG(TAG, "    DNS 1: '%s'", this->manual_ip_->dns1.toString().c_str());
    if (!is_empty(this->manual_ip_->dns2))
      ESP_LOGCONFIG(TAG, "    DNS 2: '%s'", this->manual_ip_->dns2.toString().c_str());
    ret = WiFi.config(this->manual_ip_->static_ip, this->manual_ip_->gateway, this->manual_ip_->subnet,
                      this->manual_ip_->dns1, this->manual_ip_->dns2);
    if (!ret)
      ESP_LOGE(TAG, "WiFi.config() failed!");
  }

  this->wait_for_connection();
}

void WiFiComponent::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    ESP_LOGI(TAG, "Reconnecting WiFi...");
#ifdef ARDUINO_ARCH_ESP32
    esp_wifi_connect();
#endif
#ifdef ARDUINO_ARCH_ESP8266
    wifi_station_connect();
#endif
    this->wait_for_connection();
  }
}

WiFiComponent::WiFiComponent(std::string ssid, std::string password, std::string hostname)
    : ssid_(std::move(ssid)), password_(std::move(password)), hostname_(std::move(hostname)) {}

void WiFiComponent::on_wifi_event(WiFiEvent_t event) {
  std::string event_name;
  switch (event) {
#ifdef ARDUINO_ARCH_ESP32
    case SYSTEM_EVENT_WIFI_READY:event_name = "WiFi ready";
      break;
    case SYSTEM_EVENT_SCAN_DONE:event_name = "Scan done";
      break;
    case SYSTEM_EVENT_STA_START:event_name = "STA start";
      break;
    case SYSTEM_EVENT_STA_STOP:event_name = "STA stop";
      break;
    case SYSTEM_EVENT_STA_CONNECTED:event_name = "STA connected";
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:event_name = "STA disconnected";
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:event_name = "STA auth mode change";
      break;
    case SYSTEM_EVENT_STA_GOT_IP:event_name = "STA got IP";
      break;
#endif
    default:event_name = "UNKNOWN";
      break;
  }

  ESP_LOGV(TAG, "[WiFi event] %s (%d)", event_name.c_str(), event);
}

void WiFiComponent::set_hostname(const std::string &hostname) {
  this->hostname_ = sanitize_hostname(hostname);
}

void WiFiComponent::set_manual_ip(Optional<ManualIP> manual_ip) {
  this->manual_ip_ = std::move(manual_ip);
}
const std::string &WiFiComponent::get_hostname() const {
  return this->hostname_;
}
void WiFiComponent::wait_for_connection() {
  if (WiFi.status() == WL_CONNECTED)
    return;
  wl_status_t reti = WiFi.begin(this->ssid_.c_str(), this->password_.c_str());
  if (reti == WL_CONNECT_FAILED)
    ESP_LOGE(TAG, "WiFi.begin() failed: %d", reti);
  ESP_LOGI(TAG, "Waiting for WiFi connection");
  uint32_t start = millis();
  wl_status_t status;
  while ((status = WiFi.status()) != WL_CONNECTED) {
    if (status == WL_CONNECT_FAILED || millis() - start > 10000) {
      ESP_LOGE(TAG, "    Can't connect to WiFi network, restarting...");
      WiFi.mode(WIFI_OFF);
      ESP.restart();
    }

    delay(250);
    ESP_LOGV(TAG, ". (status=%d)", status);
  }

  ESP_LOGI(TAG, "WiFi connected.");
  ESP_LOGCONFIG(TAG, "    IP Address: %s", WiFi.localIP().toString().c_str());
  ESP_LOGCONFIG(TAG, "    Subnet: %s", WiFi.subnetMask().toString().c_str());
  ESP_LOGCONFIG(TAG, "    Gateway: %s", WiFi.gatewayIP().toString().c_str());
  ESP_LOGCONFIG(TAG, "    DNS1: %s", WiFi.dnsIP(0).toString().c_str());
  ESP_LOGCONFIG(TAG, "    DNS2: %s", WiFi.dnsIP(1).toString().c_str());
}
const Optional<ManualIP> &WiFiComponent::get_manual_ip() const {
  return this->manual_ip_;
}
const std::string &WiFiComponent::get_ssid() const {
  return this->ssid_;
}
void WiFiComponent::set_ssid(const std::string &ssid) {
  assert_construction_state(this);
  this->ssid_ = ssid;
}
const std::string &WiFiComponent::get_password() const {
  return this->password_;
}
void WiFiComponent::set_password(const std::string &password) {
  assert_construction_state(this);
  this->password_ = password;
}

} // namespace esphomelib