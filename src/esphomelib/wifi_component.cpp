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

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "wifi";

float WiFiComponent::get_setup_priority() const {
  return setup_priority::WIFI;
}

void WiFiComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WiFi...");

  add_shutdown_hook([](const char *cause) {
    if (strcmp(cause, "ota") != 0) {
      WiFi.mode(WIFI_OFF);
    }
  });

#ifdef ARDUINO_ARCH_ESP32
  WiFi.onEvent(on_wifi_event);
#endif
  WiFi.persistent(false);

  if (this->has_ap()) {
    this->setup_ap_config();
  }

  if (this->has_sta()) {
    if (!WiFi.enableSTA(true)) {
      ESP_LOGE(TAG, "WiFi.enableSTA(true) failed!");
    }
#ifdef ARDUINO_ARCH_ESP32
    int32_t ret = esp_wifi_start();
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "esp_wifi_start failed: %d", ret);
    }
#endif
    if (!WiFi.setAutoConnect(false)) {
      ESP_LOGE(TAG, "WiFi.setAutoConnect(false) failed!");
    }
    if (!WiFi.setAutoReconnect(false)) {
      ESP_LOGE(TAG, "WiFi.setAutoReconnect(false) failed!");
    }
    delay(10);
    this->start_scan();
  }
}

void WiFiComponent::loop() {
  const uint32_t now = millis();

  if (this->has_sta()) {
    switch (this->state_) {
      case WIFI_COMPONENT_STATE_STA_SCANNING:
      case WIFI_COMPONENT_STATE_AP_STA_SCANNING: {
        this->check_scan_finished();
        break;
      }

      case WIFI_COMPONENT_STATE_STA_CONNECTING:
      case WIFI_COMPONENT_STATE_AP_STA_CONNECTING: {
        this->check_connecting_finished();
        break;
      }

      case WIFI_COMPONENT_STATE_STA_CONNECTED:
      case WIFI_COMPONENT_STATE_AP_STA_CONNECTED: {
        if (WiFi.status() == WL_CONNECTED) {
          this->last_connected_ = now;
        } else {
          ESP_LOGW(TAG, "WiFi Connection lost... Reconnecting...");
          this->retry_connect();
        }
        break;
      }
      default: break;
    }

    if (!this->has_ap()) {
      if (now - this->last_connected_ > this->reboot_timeout_) {
        ESP_LOGE(TAG, "Can't connect to WiFi, rebooting...");
        reboot("wifi");
      }
    }
  }
}

WiFiComponent::WiFiComponent() {
  global_wifi_component = this;
}

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

  ESP_LOGV(TAG, "[WiFi event] %s (num=%d status=%u)", event_name, event, WiFi.status());

  if (event == SYSTEM_EVENT_STA_DISCONNECTED) {
    // The arduino core has a bug where WiFi.status() is still set to connected even though
    // it received a disconnected event.
    global_wifi_component->retry_connect();
  }
}
#endif

bool WiFiComponent::has_ap() const {
  return !this->ap_.ssid.empty();
}
bool WiFiComponent::has_sta() const {
  return !this->sta_.empty();
}
void WiFiComponent::setup_ap_config() {
  ESP_LOGCONFIG(TAG, "Setting up AP...");
  bool ret = WiFi.enableAP(true);
  if (!ret) {
    ESP_LOGE(TAG, "WiFi.enableAP(true) failed");
  }

  ESP_LOGCONFIG(TAG, "    AP SSID: '%s'", this->ap_.ssid.c_str());
  ESP_LOGCONFIG(TAG, "    AP Password: '%s'", this->ap_.password.c_str());
  if (this->ap_.manual_ip.has_value()) {
    ESP_LOGCONFIG(TAG, "    AP Static IP: '%s'", this->ap_.manual_ip->static_ip.toString().c_str());
    ESP_LOGCONFIG(TAG, "    AP Gateway: '%s'", this->ap_.manual_ip->gateway.toString().c_str());
    ESP_LOGCONFIG(TAG, "    AP Subnet: '%s'", this->ap_.manual_ip->subnet.toString().c_str());

    ret = WiFi.softAPConfig(this->ap_.manual_ip->static_ip, this->ap_.manual_ip->gateway,
                            this->ap_.manual_ip->subnet);
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

  const char *passphrase = this->ap_.password.c_str();
  if (this->ap_.password.empty())
    passphrase = nullptr;
  ret = WiFi.softAP(this->ap_.ssid.c_str(), passphrase, this->ap_.channel);
  if (!ret) {
    ESP_LOGE(TAG, "WiFi.softAP() failed!");
  }

  ESP_LOGD(TAG, "WiFi AP set up.");
  ESP_LOGCONFIG(TAG, "    IP Address: %s", WiFi.softAPIP().toString().c_str());

  if (!this->has_sta()) {
    this->state_ = WIFI_COMPONENT_STATE_AP;
  }
}

void WiFiComponent::set_hostname(std::string &&hostname) {
  this->hostname_ = std::move(hostname);
}
const std::string &WiFiComponent::get_hostname() {
  return this->hostname_;
}
float WiFiComponent::get_loop_priority() const {
  return 10.0f; // before other loop components
}
void WiFiComponent::set_ap(const WiFiAp &ap) {
  this->ap_ = ap;
}
void WiFiComponent::add_sta(const WiFiAp &ap) {
  this->sta_.push_back(ap);
}
void WiFiComponent::start_scan() {
  ESP_LOGI(TAG, "Starting WiFi scan...");
  if (WiFi.scanNetworks(true, true) != WIFI_SCAN_RUNNING) {
    ESP_LOGE(TAG, "WiFi.scanNetworks() failed!");
  }

  if (this->has_ap()) {
    this->state_ = WIFI_COMPONENT_STATE_AP_STA_SCANNING;
  } else {
    this->state_ = WIFI_COMPONENT_STATE_STA_SCANNING;
  }
  this->action_started_ = millis();
}

#ifdef ARDUINO_ARCH_ESP32
const char *get_auth_mode_string(uint8_t auth_mode) {
  switch (auth_mode) {
    case WIFI_AUTH_OPEN: return "OPEN";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA+PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2+PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2+PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
    default: return "UNKNOWN";
  }
}
#endif
#ifdef ARDUINO_ARCH_ESP8266
const char *get_auth_mode_string(uint8_t auth_mode) {
  switch (auth_mode) {
    case ENC_TYPE_NONE: return "OPEN";
    case ENC_TYPE_WEP: return "WEP";
    case ENC_TYPE_TKIP: return "WPA+PSK";
    case ENC_TYPE_CCMP: return "WPA2+PSK";
    case ENC_TYPE_AUTO: return "WPA/WPA2+PSK";
    default: return "UNKNOWN";
  }
}
#endif

uint64_t bssid_ptr_to_uint64(const uint8_t *bssid) {
  return
      uint64_t(bssid[0]) << 40 |
      uint64_t(bssid[1]) << 32 |
      uint64_t(bssid[2]) << 24 |
      uint64_t(bssid[3]) << 16 |
      uint64_t(bssid[4]) << 8 |
      uint64_t(bssid[5]) << 0;
}

void uint64_to_bssid_ptr(uint64_t bssid, uint8_t *out) {
  out[0] = bssid >> 40;
  out[1] = bssid >> 32;
  out[2] = bssid >> 24;
  out[3] = bssid >> 16;
  out[4] = bssid >> 8;
  out[5] = bssid >> 0;
}

void WiFiComponent::check_scan_finished() {
  assert(this->state_ == WIFI_COMPONENT_STATE_STA_SCANNING || this->state_ == WIFI_COMPONENT_STATE_AP_STA_SCANNING);
  assert(this->has_sta());

  int8_t state = WiFi.scanComplete();
  if (state == WIFI_SCAN_RUNNING) {
    // scan not finished
    return;
  }
  if (state == WIFI_SCAN_FAILED) {
    ESP_LOGW(TAG, "WiFi Scan failed!");
    // re-try scan
    this->retry_connect();
    return;
  }
  if (state == 0) {
    // No networks found, re-try
    ESP_LOGW(TAG, "WiFi Scan finished, but no networks found!");
    this->retry_connect();
    return;
  }
  if (millis() - this->action_started_ > 30000) {
    ESP_LOGW(TAG, "Timeout while scanning for WiFi.");
    this->retry_connect();
    return;
  }

  ESP_LOGI(TAG, "WiFi scan finished, found %d networks:", state);
  yield();

  std::string best_ssid;
  std::string best_password;
  int32_t best_rssi = INT32_MIN;
  int32_t best_channel = 0;
  uint64_t best_bssid = 0;
  optional<ManualIP> best_manual_ip;
  bool found = false;

  for (int i = 0; i < state; i++) {
    String ssid;
    uint8_t auth_mode;
    int32_t rssi;
    uint8_t *bssid_ptr;
    int32_t channel;
#ifdef ARDUINO_ARCH_ESP32
    WiFi.getNetworkInfo(i, ssid, auth_mode, rssi, bssid_ptr, channel);
#endif
#ifdef ARDUINO_ARCH_ESP8266
    bool hidden;
    WiFi.getNetworkInfo(i, ssid, auth_mode, rssi, bssid_ptr, channel, hidden);
#endif
    uint64_t bssid = bssid_ptr_to_uint64(bssid_ptr);

    bool matches = false;
    for (const WiFiAp &ap : this->sta_) {
      if (ap.matches(ssid.c_str(), bssid, auth_mode, channel)) {
        matches = true;
        if (rssi > best_rssi) {
          best_ssid = ssid.c_str();
          best_password = ap.password;
          best_rssi = rssi;
          best_bssid = bssid;
          best_channel = channel;
          best_manual_ip = ap.manual_ip;
          found = true;
        }
      }
    }

    if (matches) {
      ESP_LOGI(TAG, " => '%s'", ssid.c_str());
    } else {
      ESP_LOGI(TAG, " -  '%s'", ssid.c_str());
    }
    ESP_LOGD(TAG, "    Authentication Mode: %s", get_auth_mode_string(auth_mode));
    ESP_LOGD(TAG, "    RSSI: %d", rssi);
    ESP_LOGD(TAG, "    Channel: %d", channel);
    ESP_LOGD(TAG, "    BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
             bssid_ptr[0], bssid_ptr[1], bssid_ptr[2], bssid_ptr[3], bssid_ptr[4], bssid_ptr[5]);
  }

  WiFi.scanDelete();

  if (!found) {
    ESP_LOGW(TAG, "WiFi scan found no configured network!");
    this->retry_connect();
    return;
  }
  uint8_t bssid[6];
  uint64_to_bssid_ptr(best_bssid, bssid);

  ESP_LOGI(TAG, "Connecting to '%s' with BSSID=%02X:%02X:%02X:%02X:%02X:%02X...",
           best_ssid.c_str(), bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

  bool ret;
  if (best_manual_ip.has_value()) {
    ret = WiFi.config(best_manual_ip->static_ip, best_manual_ip->gateway, best_manual_ip->subnet,
                      best_manual_ip->dns1, best_manual_ip->dns2);
  } else {
    ret = WiFi.config(0u, 0u, 0u, 0u, 0u);
  }

  if (!ret) {
    ESP_LOGW(TAG, "WiFi.config() failed!");
  }

  if (!this->hostname_.empty()) {
    ESP_LOGCONFIG(TAG, "    STA Hostname: '%s'", this->hostname_.c_str());
#ifdef ARDUINO_ARCH_ESP32
    ret = WiFi.setHostname(this->hostname_.c_str());
#endif
#ifdef ARDUINO_ARCH_ESP8266
    ret = WiFi.hostname(this->hostname_.c_str());
#endif
    if (!ret)
      ESP_LOGE(TAG, "WiFi.hostname() failed!");
  }

  const char *passphrase = best_password.c_str();
  if (best_password.empty()) {
    passphrase = nullptr;
  }

  WiFi.begin(best_ssid.c_str(), passphrase, best_channel, bssid);

  if (this->has_ap()) {
    this->state_ = WIFI_COMPONENT_STATE_AP_STA_CONNECTING;
  } else {
    this->state_ = WIFI_COMPONENT_STATE_STA_CONNECTING;
  }
  this->action_started_ = millis();
}

void WiFiComponent::check_connecting_finished() {
  assert(this->state_ == WIFI_COMPONENT_STATE_STA_CONNECTING || this->state_ == WIFI_COMPONENT_STATE_AP_STA_CONNECTING);
  assert(this->has_sta());

  wl_status_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    ESP_LOGI(TAG, "WiFi connected.");
    uint8_t *bssid = WiFi.BSSID();
    ESP_LOGCONFIG(TAG, "    SSID: %s", WiFi.SSID().c_str());
    ESP_LOGCONFIG(TAG, "    BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
                  bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    ESP_LOGCONFIG(TAG, "    Channel: %d", WiFi.channel());
    ESP_LOGCONFIG(TAG, "    IP Address: %s", WiFi.localIP().toString().c_str());
    ESP_LOGCONFIG(TAG, "    Subnet: %s", WiFi.subnetMask().toString().c_str());
    ESP_LOGCONFIG(TAG, "    Gateway: %s", WiFi.gatewayIP().toString().c_str());
    ESP_LOGCONFIG(TAG, "    DNS1: %s", WiFi.dnsIP(0).toString().c_str());
    ESP_LOGCONFIG(TAG, "    DNS2: %s", WiFi.dnsIP(1).toString().c_str());

    if (this->has_ap()) {
      ESP_LOGD(TAG, "Disabling AP...");
      WiFi.enableAP(false);
      this->state_ = WIFI_COMPONENT_STATE_AP_STA_CONNECTED;
    } else {
      this->state_ = WIFI_COMPONENT_STATE_STA_CONNECTED;
    }
    return;
  }

  if (status == WL_NO_SSID_AVAIL) {
    ESP_LOGW(TAG, "WiFi network can not be found anymore.");
    this->retry_connect();
    return;
  }

  if (status == WL_CONNECT_FAILED) {
    ESP_LOGW(TAG, "Connecting to WiFi network failed. Are the credentials wrong?");
    this->retry_connect();
    return;
  }

  if (millis() - this->action_started_ > 30000) {
    ESP_LOGW(TAG, "Timeout while connecting to WiFi.");
    this->retry_connect();
    return;
  }

  if (status == WL_IDLE_STATUS || status == WL_DISCONNECTED) {
    // WL_DISCONNECTED is set while not connected yet.
    // WL_IDLE_STATUS is set while we're waiting for the IP address.
    return;
  }

  ESP_LOGW(TAG, "WiFi Unknown connection status %d", status);
  delay(100);
}

void WiFiComponent::retry_connect() {
  if (this->num_retried_ > 5) {
    // If retry failed for more than 5 times, let's restart STA
    ESP_LOGW(TAG, "Restarting WiFi adapter...");
    if (!WiFi.enableSTA(false)) {
      ESP_LOGE(TAG, "WiFi.enableSTA(false) failed!");
    }
    delay(100);
    this->num_retried_ = 0;
  } else {
    this->num_retried_++;
  }
  if (this->has_ap()) {
    switch (this->state_) {
      case WIFI_COMPONENT_STATE_AP_STA_CONNECTED:
        // Re-enable AP
        this->setup_ap_config();
        break;
      default:
        break;
    }
  }
  this->start_scan();
}

bool WiFiComponent::can_proceed() {
  if (this->has_ap())
    return true;

  return this->state_ == WIFI_COMPONENT_STATE_STA_CONNECTED;
}

bool WiFiAp::matches(const char *ssid, uint64_t bssid, uint8_t auth_mode, int32_t channel) const {
  if (!this->ssid.empty() && this->ssid != ssid) {
    return false;
  }
  if (this->bssid != 0 && bssid != this->bssid) {
    return false;
  }
  if (this->channel != -1 && this->channel != channel) {
    return false;
  }
#ifdef ARDUINO_ARCH_ESP32
  return this->password.empty() == (auth_mode == WIFI_AUTH_OPEN);
#endif
#ifdef ARDUINO_ARCH_ESP8266
  return this->password.empty() == (auth_mode == ENC_TYPE_NONE);
#endif
}

WiFiComponent *global_wifi_component;

ESPHOMELIB_NAMESPACE_END
