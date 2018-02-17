//
// Created by Otto Winter on 25.11.17.
//

#include "wifi_component.h"
#include "helpers.h"

#include <esp_log.h>
#include <esp32-hal.h>
#include <WiFi.h>
#include <esp_wifi.h>

namespace esphomelib {

static const char *TAG = "wifi_component";

float WiFiComponent::get_setup_priority() const {
  return setup_priority::WIFI;
}

void WiFiComponent::setup() {
  ESP_LOGD(TAG, "Setting up WiFi...");
  WiFi.onEvent(on_wifi_event);
  delay(10);
  ESP_LOGD(TAG, "    Connecting to '%s'", this->ssid_.c_str());

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);

  if (!this->hostname_.empty()) {
    ESP_LOGV(TAG, "    Hostname: '%s'", this->hostname_.c_str());
    WiFi.setHostname(this->hostname_.c_str());
  }

  if (this->manual_ip_) {
    ESP_LOGV(TAG, "    Static IP: '%s'", this->manual_ip_->static_ip.toString().c_str());
    ESP_LOGV(TAG, "    Gateway: '%s'", this->manual_ip_->gateway.toString().c_str());
    ESP_LOGV(TAG, "    Subnet: '%s'", this->manual_ip_->subnet.toString().c_str());
    if (!is_empty(this->manual_ip_->dns1))
      ESP_LOGV(TAG, "    DNS 1: '%s'", this->manual_ip_->dns1.toString().c_str());
    if (!is_empty(this->manual_ip_->dns2))
      ESP_LOGV(TAG, "    DNS 2: '%s'", this->manual_ip_->dns2.toString().c_str());
    WiFi.config(this->manual_ip_->static_ip, this->manual_ip_->gateway, this->manual_ip_->subnet,
                this->manual_ip_->dns1, this->manual_ip_->dns2);
  }

  WiFi.begin(this->ssid_.c_str(), this->password_.c_str());
  this->wait_for_connection();
}

void WiFiComponent::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    ESP_LOGD(TAG, "Reconnecting WiFi...");
    int return_value = esp_wifi_connect();
    if (!return_value)
      ESP_LOGW(TAG, "Reconnecting WiFi error: %d", return_value);
    this->wait_for_connection();
  }
}

WiFiComponent::WiFiComponent(std::string ssid, std::string password, std::string hostname)
    : ssid_(std::move(ssid)), password_(std::move(password)), hostname_(std::move(hostname)) {}

void WiFiComponent::on_wifi_event(WiFiEvent_t event) {
  std::string event_name;
  switch (event) {
    case SYSTEM_EVENT_WIFI_READY:event_name = "WiFi ready";
      break;
    case SYSTEM_EVENT_SCAN_DONE:event_name = "Scan done";
      break;
    case SYSTEM_EVENT_STA_START:event_name = "STA start";
      break;
    case SYSTEM_EVENT_STA_STOP:event_name = "STA stop";
      break;
    case SYSTEM_EVENT_STA_CONNECTED:event_name = "STA IsConnected";
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:event_name = "STA disconnected";
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:event_name = "STA auth mode changed";
      break;
    case SYSTEM_EVENT_STA_GOT_IP:event_name = "STA got IP";
      break;
    default:event_name = "";
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
  assert_setup(this);
  ESP_LOGV(TAG, "Waiting for WiFi connection");
  uint32_t start = millis();
  wl_status_t status;
  while ((status = WiFi.status()) != WL_CONNECTED) {
    if (status == WL_CONNECT_FAILED || millis() - start > 10000) {
      ESP_LOGE(TAG, "    Can't connect to WiFi network, restarting...");
      WiFi.disconnect(true);
      esp_wifi_stop();
      esp_restart();
    }

    delay(500);
    ESP_LOGV(TAG, ". (status=%d)", status);
  }

  ESP_LOGV(TAG, "    WiFi connected.");
  ESP_LOGV(TAG, "    IP Address: %s", WiFi.localIP().toString().c_str());
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