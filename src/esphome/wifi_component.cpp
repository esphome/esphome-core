#include "esphome/wifi_component.h"

#ifdef ARDUINO_ARCH_ESP32
  #include <esp_wifi.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include <user_interface.h>
#endif

#include <utility>
#include <algorithm>
#include "lwip/err.h"
#include "lwip/dns.h"

#include "esphome/helpers.h"
#include "esphome/log.h"
#include "esphome/esphal.h"
#include "esphome/util.h"

ESPHOME_NAMESPACE_BEGIN

static const char *TAG = "wifi";

float WiFiComponent::get_setup_priority() const {
  return setup_priority::WIFI;
}

void WiFiComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WiFi...");

  this->wifi_register_callbacks_();

  bool ret = this->wifi_mode_(this->has_sta(), false);
  if (!ret) {
    this->mark_failed();
    return;
  }

  if (this->has_sta()) {
    this->wifi_disable_auto_connect_();
    delay(10);

    this->wifi_apply_power_save_();

    if (this->fast_connect_) {
      this->selected_ap_ = this->sta_[0];
      this->start_connecting(this->selected_ap_, false);
    } else {
      this->start_scanning();
    }
  } else if (this->has_ap()) {
    this->setup_ap_config();
  }

  this->wifi_apply_hostname_();
  network_setup_mdns();
}

void WiFiComponent::loop() {
  const uint32_t now = millis();

  if (this->has_sta()) {
    switch (this->state_) {
      case WIFI_COMPONENT_STATE_COOLDOWN: {
        this->status_set_warning();
        if (millis() - this->action_started_ > 5000) {
          if (this->fast_connect_) {
            this->start_connecting(this->sta_[0], false);
          } else {
            this->start_scanning();
          }
        }
        break;
      }
      case WIFI_COMPONENT_STATE_STA_SCANNING: {
        this->status_set_warning();
        this->check_scanning_finished();
        break;
      }
      case WIFI_COMPONENT_STATE_STA_CONNECTING:
      case WIFI_COMPONENT_STATE_STA_CONNECTING_2: {
        this->status_set_warning();
        this->check_connecting_finished();
        break;
      }

      case WIFI_COMPONENT_STATE_STA_CONNECTED: {
        if (!this->is_connected()) {
          ESP_LOGW(TAG, "WiFi Connection lost... Reconnecting...");
          this->retry_connect();
        } else {
          this->status_clear_warning();
          this->last_connected_ = now;
        }
        break;
      }
      case WIFI_COMPONENT_STATE_OFF:
      case WIFI_COMPONENT_STATE_AP:
        break;
    }

    if (!this->has_ap() && this->reboot_timeout_ != 0) {
      if (now - this->last_connected_ > this->reboot_timeout_) {
        ESP_LOGE(TAG, "Can't connect to WiFi, rebooting...");
        reboot("wifi");
      }
    }
  }

  network_tick_mdns();
}

WiFiComponent::WiFiComponent() {
  global_wifi_component = this;
}

bool WiFiComponent::has_ap() const {
  return !this->ap_.get_ssid().empty();
}
bool WiFiComponent::has_sta() const {
  return !this->sta_.empty();
}
void WiFiComponent::set_fast_connect(bool fast_connect) {
  this->fast_connect_ = fast_connect;
}
IPAddress WiFiComponent::get_ip_address() {
  if (this->has_sta())
    return this->wifi_sta_ip_();
  if (this->has_ap())
    return this->wifi_soft_ap_ip_();
  return IPAddress();
}
std::string WiFiComponent::get_use_address() const {
  if (this->use_address_.empty()) {
    return get_app_name() + ".local";
  }
  return this->use_address_;
}
void WiFiComponent::set_use_address(const std::string &use_address) {
  this->use_address_ = use_address;
}
void WiFiComponent::setup_ap_config() {
  this->wifi_mode_({}, true);

  if (this->ap_setup_)
    return;

  ESP_LOGCONFIG(TAG, "Setting up AP...");

  ESP_LOGCONFIG(TAG, "  AP SSID: '%s'", this->ap_.get_ssid().c_str());
  ESP_LOGCONFIG(TAG, "  AP Password: '%s'", this->ap_.get_password().c_str());
  if (this->ap_.get_manual_ip().has_value()) {
    ESP_LOGCONFIG(TAG, "  AP Static IP: '%s'", this->ap_.get_manual_ip()->static_ip.toString().c_str());
    ESP_LOGCONFIG(TAG, "  AP Gateway: '%s'", this->ap_.get_manual_ip()->gateway.toString().c_str());
    ESP_LOGCONFIG(TAG, "  AP Subnet: '%s'", this->ap_.get_manual_ip()->subnet.toString().c_str());
  }

  this->ap_setup_ = this->wifi_start_ap_(this->ap_);
  ESP_LOGCONFIG(TAG, "  IP Address: %s", this->wifi_soft_ap_ip_().toString().c_str());

  if (!this->has_sta()) {
    this->state_ = WIFI_COMPONENT_STATE_AP;
  }
}

float WiFiComponent::get_loop_priority() const {
  return 10.0f; // before other loop components
}
void WiFiComponent::set_ap(const WiFiAP &ap) {
  this->ap_ = ap;
}
void WiFiComponent::add_sta(const WiFiAP &ap) {
  this->sta_.push_back(ap);
}

std::string format_mac_addr(const uint8_t mac[6]) {
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return buf;
}

void WiFiComponent::start_connecting(const WiFiAP &ap, bool two) {
  ESP_LOGI(TAG, "WiFi Connecting to '%s'...", ap.get_ssid().c_str());

  if (!this->wifi_sta_connect_(ap)) {
    ESP_LOGE(TAG, "wifi_sta_connect_ failed!");
    this->retry_connect();
    return;
  }

  if (!two) {
    this->state_ = WIFI_COMPONENT_STATE_STA_CONNECTING;
  } else {
    this->state_ = WIFI_COMPONENT_STATE_STA_CONNECTING_2;
  }
  this->action_started_ = millis();
}

void print_signal_bars(int8_t rssi, char *buf) {
  // LOWER ONE QUARTER BLOCK
  // Unicode: U+2582, UTF-8: E2 96 82
  // LOWER HALF BLOCK
  // Unicode: U+2584, UTF-8: E2 96 84
  // LOWER THREE QUARTERS BLOCK
  // Unicode: U+2586, UTF-8: E2 96 86
  // FULL BLOCK
  // Unicode: U+2588, UTF-8: E2 96 88
  if (rssi >= -50) {
    sprintf(buf, "\033[0;32m" // green
                 "\xe2\x96\x82"
                 "\xe2\x96\x84"
                 "\xe2\x96\x86"
                 "\xe2\x96\x88"
                 "\033[0m");
  } else if (rssi >= -65) {
    sprintf(buf, "\033[0;33m" // yellow
                 "\xe2\x96\x82"
                 "\xe2\x96\x84"
                 "\xe2\x96\x86"
                 "\033[0;37m"
                 "\xe2\x96\x88"
                 "\033[0m");
  } else if (rssi >= -85) {
    sprintf(buf, "\033[0;33m" // yellow
                 "\xe2\x96\x82"
                 "\xe2\x96\x84"
                 "\033[0;37m"
                 "\xe2\x96\x86"
                 "\xe2\x96\x88"
                 "\033[0m");
  } else {
    sprintf(buf, "\033[0;31m" // red
                 "\xe2\x96\x82"
                 "\033[0;37m"
                 "\xe2\x96\x84"
                 "\xe2\x96\x86"
                 "\xe2\x96\x88"
                 "\033[0m");
  }
}

void WiFiComponent::print_connect_params_() {
  uint8_t *bssid = WiFi.BSSID();
  ESP_LOGCONFIG(TAG, "  SSID: " LOG_SECRET("'%s'"), WiFi.SSID().c_str());
  ESP_LOGCONFIG(TAG, "  IP Address: %s", WiFi.localIP().toString().c_str());
  ESP_LOGCONFIG(TAG, "  BSSID: " LOG_SECRET("%02X:%02X:%02X:%02X:%02X:%02X"),
                bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  ESP_LOGCONFIG(TAG, "  Hostname: '%s'", get_app_name().c_str());
  char signal_bars[50];
  int8_t rssi = WiFi.RSSI();
  print_signal_bars(rssi, signal_bars);
  ESP_LOGCONFIG(TAG, "  Signal strength: %d dB %s", rssi, signal_bars);
  ESP_LOGCONFIG(TAG, "  Channel: %d", WiFi.channel());
  ESP_LOGCONFIG(TAG, "  Subnet: %s", WiFi.subnetMask().toString().c_str());
  ESP_LOGCONFIG(TAG, "  Gateway: %s", WiFi.gatewayIP().toString().c_str());
  ESP_LOGCONFIG(TAG, "  DNS1: %s", WiFi.dnsIP(0).toString().c_str());
  ESP_LOGCONFIG(TAG, "  DNS2: %s", WiFi.dnsIP(1).toString().c_str());
}

void WiFiComponent::start_scanning() {
  this->action_started_ = millis();
  ESP_LOGD(TAG, "Starting scan...");
  this->wifi_scan_start_();
  this->state_ = WIFI_COMPONENT_STATE_STA_SCANNING;
}

void WiFiComponent::check_scanning_finished() {
  if (!this->scan_done_) {
    if (millis() - this->action_started_ > 30000) {
      ESP_LOGE(TAG, "Scan timeout!");
      this->retry_connect();
    }
    return;
  }
  this->scan_done_ = false;

  ESP_LOGD(TAG, "Found networks:");
  if (this->scan_result_.empty()) {
    ESP_LOGD(TAG, "  No network found!");
    this->retry_connect();
    return;
  }

  for (auto &res : this->scan_result_) {
    for (auto &ap : this->sta_) {
      if (res.matches(ap)) {
        res.set_matches(true);
        break;
      }
    }
  }

  std::stable_sort(this->scan_result_.begin(), this->scan_result_.end(), [this](const WiFiScanResult &a, const WiFiScanResult &b) {
    if (a.get_matches() && !b.get_matches())
      return true;
    if (!a.get_matches() && b.get_matches())
      return false;

    return a.get_rssi() > b.get_rssi();
  });

  for (auto &res : this->scan_result_) {
    char bssid_s[18];
    auto bssid = res.get_bssid();
    sprintf(bssid_s, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    char signal_bars[50];
    print_signal_bars(res.get_rssi(), signal_bars);

    if (res.get_matches()) {
      ESP_LOGI(TAG, "- '%s' " LOG_SECRET("(%s) ") "%s", res.get_ssid().c_str(), bssid_s, signal_bars);
      ESP_LOGD(TAG, "    Channel: %u", res.get_channel());
      ESP_LOGD(TAG, "    RSSI: %d dB", res.get_rssi());
    } else {
      ESP_LOGD(TAG, "- " LOG_SECRET("'%s'") " " LOG_SECRET("(%s) ") "%s",
          res.get_ssid().c_str(), bssid_s, signal_bars);
    }
  }

  if (!this->scan_result_[0].get_matches()) {
    ESP_LOGW(TAG, "No matching network found!");
    this->retry_connect();
    return;
  }

  WiFiAP ap;
  WiFiScanResult scan_res = this->scan_result_[0];
  ap.set_ssid(scan_res.get_ssid());
  ap.set_bssid(scan_res.get_bssid());
  ap.set_channel(scan_res.get_channel());
  for (auto &ap2 : this->sta_) {
    if (scan_res.matches(ap2)) {
      if (ap.get_ssid().empty()) {
        ap.set_ssid(ap2.get_ssid());
      }
      ap.set_password(ap2.get_password());
      ap.set_manual_ip(ap2.get_manual_ip());
      break;
    }
  }

  yield();

  this->selected_ap_ = ap;
  this->start_connecting(ap, false);
}

void WiFiComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "WiFi:");
  this->print_connect_params_();
}

void WiFiComponent::check_connecting_finished() {
  wl_status_t status = this->wifi_sta_status_();

  if (status == WL_CONNECTED) {
    ESP_LOGI(TAG, "WiFi connected!");
    this->print_connect_params_();

    if (this->has_ap()) {
      ESP_LOGD(TAG, "Disabling AP...");
      this->wifi_mode_({}, false);
    }
    this->state_ = WIFI_COMPONENT_STATE_STA_CONNECTED;
    this->num_retried_ = 0;
    return;
  }

  uint32_t now = millis();
  if (now - this->action_started_ > 30000) {
    ESP_LOGW(TAG, "Timeout while connecting to WiFi.");
    this->retry_connect();
    return;
  }

  if (this->error_from_callback_) {
    ESP_LOGW(TAG, "Error while connecting to network.");
    this->retry_connect();
    return;
  }

  if (status == WL_IDLE_STATUS || status == WL_DISCONNECTED || status == WL_CONNECTION_LOST) {
    // WL_DISCONNECTED is set while not connected yet.
    // WL_IDLE_STATUS is set while we're waiting for the IP address.
    // WL_CONNECTION_LOST happens on the ESP32
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

  ESP_LOGW(TAG, "WiFi Unknown connection status %d", status);
}

void WiFiComponent::retry_connect() {
  if (this->num_retried_ > 5 || this->error_from_callback_) {
    // If retry failed for more than 5 times, let's restart STA
    ESP_LOGW(TAG, "Restarting WiFi adapter...");
    this->wifi_mode_(false, {});
    delay(100);
    this->num_retried_ = 0;
  } else {
    this->num_retried_++;
  }
  this->error_from_callback_ = false;
  if (this->state_ == WIFI_COMPONENT_STATE_STA_CONNECTING) {
    yield();
    this->state_ = WIFI_COMPONENT_STATE_STA_CONNECTING_2;
    this->start_connecting(this->selected_ap_, true);
    return;
  }

  if (this->has_ap()) {
    this->setup_ap_config();
  }
  this->state_ = WIFI_COMPONENT_STATE_COOLDOWN;
  this->action_started_ = millis();
}

bool WiFiComponent::can_proceed() {
  if (this->has_ap() && !this->has_sta()) {
    return true;
  }
  return this->is_connected();
}
void WiFiComponent::set_reboot_timeout(uint32_t reboot_timeout) {
  this->reboot_timeout_ = reboot_timeout;
}
bool WiFiComponent::is_connected() {
  return this->state_ == WIFI_COMPONENT_STATE_STA_CONNECTED &&
         this->wifi_sta_status_() == WL_CONNECTED && !this->error_from_callback_;
}
bool WiFiComponent::ready_for_ota() {
  if (this->has_ap())
    return true;
  return this->is_connected();
}
void WiFiComponent::set_power_save_mode(WiFiPowerSaveMode power_save) {
  this->power_save_ = power_save;
}

bool sta_field_equal(const uint8_t *field_a, const uint8_t *field_b, int len) {
  for (int i = 0; i < len; i++) {
    uint8_t a = field_a[i];
    uint8_t b = field_b[i];
    if (a == b && a == 0)
      break;
    if (a == b)
      continue;

    return false;
  }

  return true;
}

#ifdef ARDUINO_ARCH_ESP8266
bool WiFiComponent::wifi_mode_(optional<bool> sta, optional<bool> ap) {
  uint8_t current_mode = wifi_get_opmode();
  bool current_sta = current_mode & 0b01;
  bool current_ap = current_mode & 0b10;
  bool sta_ = sta.value_or(current_sta);
  bool ap_ = ap.value_or(current_ap);
  if (current_sta == sta_ && current_ap == ap_)
    return true;

  if (sta_ && !current_sta) {
    ESP_LOGV(TAG, "Enabling STA.");
  } else if (!sta_ && current_sta) {
    ESP_LOGV(TAG, "Disabling STA.");
    // Stop DHCP client when disabling STA
    // See https://github.com/esp8266/Arduino/pull/5703
    wifi_station_dhcpc_stop();
  }
  if (ap_ && !current_ap) {
    ESP_LOGV(TAG, "Enabling AP.");
  } else if (!ap_ && current_ap) {
    ESP_LOGV(TAG, "Disabling AP.");
  }

  ETS_UART_INTR_DISABLE();
  uint8_t mode = 0;
  if (sta_) mode |= 0b01;
  if (ap_) mode |= 0b10;
  bool ret = wifi_set_opmode_current(mode);
  ETS_UART_INTR_ENABLE();

  if (!ret) {
    ESP_LOGW(TAG, "Setting WiFi mode failed!");
  }

  return ret;
}
bool WiFiComponent::wifi_disable_auto_connect_() {
  bool ret1, ret2;
  ETS_UART_INTR_DISABLE();
  ret1 = wifi_station_set_auto_connect(0);
  ret2 = wifi_station_set_reconnect_policy(false);
  ETS_UART_INTR_ENABLE();

  if (!ret1 || !ret2) {
    ESP_LOGV(TAG, "Disabling Auto-Connect failed!");
  }

  return ret1 && ret2;
}
bool WiFiComponent::wifi_apply_power_save_() {
  sleep_type_t power_save;
  switch (this->power_save_) {
    case WIFI_POWER_SAVE_LIGHT:
      power_save = LIGHT_SLEEP_T;
      break;
    case WIFI_POWER_SAVE_HIGH:
      power_save = MODEM_SLEEP_T;
      break;
    case WIFI_POWER_SAVE_NONE:
    default:
      power_save = NONE_SLEEP_T;
      break;
  }
  return wifi_set_sleep_type(power_save);
}
bool WiFiComponent::wifi_sta_ip_config_(optional<ManualIP> manual_ip) {
  // enable STA
  if (!this->wifi_mode_(true, {}))
    return false;

  enum dhcp_status dhcp_status = wifi_station_dhcpc_status();
  if (!manual_ip.has_value()) {
    // Use DHCP client
    if (dhcp_status != DHCP_STARTED) {
      bool ret = wifi_station_dhcpc_start();
      if (!ret) {
        ESP_LOGV(TAG, "Starting DHCP client failed!");
      }
      return ret;
    }
    return true;
  }

  bool ret = true;

  struct ip_info info;
  info.ip.addr = static_cast<uint32_t>(manual_ip->static_ip);
  info.gw.addr = static_cast<uint32_t>(manual_ip->gateway);
  info.netmask.addr = static_cast<uint32_t>(manual_ip->subnet);

  if (dhcp_status == DHCP_STARTED) {
    bool dhcp_stop_ret = wifi_station_dhcpc_stop();
    if (!dhcp_stop_ret) {
      ESP_LOGV(TAG, "Stopping DHCP client failed!");
      ret = false;
    }
  }
  bool wifi_set_info_ret = wifi_set_ip_info(STATION_IF, &info);
  if (!wifi_set_info_ret) {
    ESP_LOGV(TAG, "Setting manual IP info failed!");
    ret = false;
  }

  ip_addr_t dns;
  if (!is_empty(manual_ip->dns1)) {
    dns.addr = static_cast<uint32_t>(manual_ip->dns1);
    dns_setserver(0, &dns);
  }
  if (!is_empty(manual_ip->dns2)) {
    dns.addr = static_cast<uint32_t>(manual_ip->dns2);
    dns_setserver(1, &dns);
  }

  return ret;
}


IPAddress WiFiComponent::wifi_sta_ip_() {
  if (!this->has_sta())
    return IPAddress();
  struct ip_info ip;
  wifi_get_ip_info(STATION_IF, &ip);
  return IPAddress(ip.ip.addr);
}
bool WiFiComponent::wifi_apply_hostname_() {
  bool ret = wifi_station_set_hostname(const_cast<char *>(get_app_name().c_str()));
  if (!ret) {
    ESP_LOGV(TAG, "Setting WiFi Hostname failed!");
  }
  return ret;
}

bool WiFiComponent::wifi_sta_connect_(WiFiAP ap) {
  // enable STA
  if (!this->wifi_mode_(true, {}))
    return false;

  ETS_UART_INTR_DISABLE();
  wifi_station_disconnect();
  ETS_UART_INTR_ENABLE();

  struct station_config conf;
  memset(&conf, 0, sizeof(conf));
  strcpy(reinterpret_cast<char *>(conf.ssid), ap.get_ssid().c_str());
  strcpy(reinterpret_cast<char *>(conf.password), ap.get_password().c_str());

  if (ap.get_bssid().has_value()) {
    conf.bssid_set = 1;
    memcpy(conf.bssid, ap.get_bssid()->data(), 6);
  } else {
    conf.bssid_set = 0;
  }

  if (ap.get_ssid().empty()) {
    conf.threshold.authmode = AUTH_OPEN;
  } else {
    conf.threshold.authmode = AUTH_WPA_PSK;
  }
  conf.threshold.rssi = -127;

  ETS_UART_INTR_DISABLE();
  bool ret = wifi_station_set_config_current(&conf);
  ETS_UART_INTR_ENABLE();

  if (!ret) {
    ESP_LOGV(TAG, "Setting WiFi Station config failed!");
    return false;
  }

  if (!this->wifi_sta_ip_config_(ap.get_manual_ip())) {
    return false;
  }

  ETS_UART_INTR_DISABLE();
  ret = wifi_station_connect();
  ETS_UART_INTR_ENABLE();
  if (!ret) {
    ESP_LOGV(TAG, "wifi_station_connect failed!");
    return false;
  }

  if (ap.get_channel().has_value()) {
    ret = wifi_set_channel(*ap.get_channel());
    if (!ret) {
      ESP_LOGV(TAG, "wifi_set_channel failed!");
      return false;
    }
  }

  return true;
}

class WiFiMockClass : public ESP8266WiFiGenericClass {
 public:
  static void _event_callback(void *event) {
    ESP8266WiFiGenericClass::_eventCallback(event);
  }
};

const char *get_auth_mode_str(uint8_t mode) {
  switch (mode) {
    case AUTH_OPEN: return "OPEN";
    case AUTH_WEP: return "WEP";
    case AUTH_WPA_PSK: return "WPA PSK";
    case AUTH_WPA2_PSK: return "WPA2 PSK";
    case AUTH_WPA_WPA2_PSK: return "WPA/WPA2 PSK";
    default: return "UNKNOWN";
  }
}
std::string format_ip_addr(struct ip_addr ip) {
  char buf[20];
  sprintf(buf, "%u.%u.%u.%u", uint8_t(ip.addr >> 0), uint8_t(ip.addr >> 8), uint8_t(ip.addr >> 16), uint8_t(ip.addr >> 24));
  return buf;
}
const char *get_op_mode_str(uint8_t mode) {
  switch (mode) {
    case WIFI_OFF: return "OFF";
    case WIFI_STA: return "STA";
    case WIFI_AP: return "AP";
    case WIFI_AP_STA: return "AP+STA";
    default: return "UNKNOWN";
  }
}
const char *get_disconnect_reason_str(uint8_t reason) {
  switch (reason) {
    case REASON_AUTH_EXPIRE: return "Auth Expired";
    case REASON_AUTH_LEAVE: return "Auth Leave";
    case REASON_ASSOC_EXPIRE: return "Association Expired";
    case REASON_ASSOC_TOOMANY: return "Too Many Associations";
    case REASON_NOT_AUTHED: return "Not Authenticated";
    case REASON_NOT_ASSOCED: return "Not Associated";
    case REASON_ASSOC_LEAVE: return "Association Leave";
    case REASON_ASSOC_NOT_AUTHED: return "Association not Authenticated";
    case REASON_DISASSOC_PWRCAP_BAD: return "Disassociate Power Cap Bad";
    case REASON_DISASSOC_SUPCHAN_BAD: return "Disassociate Supported Channel Bad";
    case REASON_IE_INVALID: return "IE Invalid";
    case REASON_MIC_FAILURE: return "Mic Failure";
    case REASON_4WAY_HANDSHAKE_TIMEOUT: return "4-Way Handshake Timeout";
    case REASON_GROUP_KEY_UPDATE_TIMEOUT: return "Group Key Update Timeout";
    case REASON_IE_IN_4WAY_DIFFERS: return "IE In 4-Way Handshake Differs";
    case REASON_GROUP_CIPHER_INVALID: return "Group Cipher Invalid";
    case REASON_PAIRWISE_CIPHER_INVALID: return "Pairwise Cipher Invalid";
    case REASON_AKMP_INVALID: return "AKMP Invalid";
    case REASON_UNSUPP_RSN_IE_VERSION: return "Unsupported RSN IE version";
    case REASON_INVALID_RSN_IE_CAP: return "Invalid RSN IE Cap";
    case REASON_802_1X_AUTH_FAILED: return "802.1x Authentication Failed";
    case REASON_CIPHER_SUITE_REJECTED: return "Cipher Suite Rejected";
    case REASON_BEACON_TIMEOUT: return "Beacon Timeout";
    case REASON_NO_AP_FOUND: return "AP Not Found";
    case REASON_AUTH_FAIL: return "Authentication Failed";
    case REASON_ASSOC_FAIL: return "Association Failed";
    case REASON_HANDSHAKE_TIMEOUT: return "Handshake Failed";
    case REASON_UNSPECIFIED:
    default:
      return "Unspecified";
  }
}

void WiFiComponent::wifi_event_callback_(System_Event_t *event) {
  switch (event->event) {
    case EVENT_STAMODE_CONNECTED: {
      auto it = event->event_info.connected;
      char buf[33];
      memcpy(buf, it.ssid, it.ssid_len);
      buf[it.ssid_len] = '\0';
      ESP_LOGV(TAG, "Event: Connected ssid='%s' bssid=%s channel=%u",
               buf, format_mac_addr(it.bssid).c_str(), it.channel);
      break;
    }
    case EVENT_STAMODE_DISCONNECTED: {
      auto it = event->event_info.disconnected;
      char buf[33];
      memcpy(buf, it.ssid, it.ssid_len);
      buf[it.ssid_len] = '\0';
      ESP_LOGW(TAG, "Event: Disconnected ssid='%s' bssid=%s reason='%s'",
               buf, format_mac_addr(it.bssid).c_str(), get_disconnect_reason_str(it.reason));
      break;
    }
    case EVENT_STAMODE_AUTHMODE_CHANGE: {
      auto it = event->event_info.auth_change;
      ESP_LOGV(TAG, "Event: Changed AuthMode old=%s new=%s",
               get_auth_mode_str(it.old_mode), get_auth_mode_str(it.new_mode));
      break;
    }
    case EVENT_STAMODE_GOT_IP: {
      auto it = event->event_info.got_ip;
      ESP_LOGV(TAG, "Event: Got IP static_ip=%s gateway=%s netmask=%s",
               format_ip_addr(it.ip).c_str(), format_ip_addr(it.gw).c_str(), format_ip_addr(it.mask).c_str());
      break;
    }
    case EVENT_STAMODE_DHCP_TIMEOUT: {
      ESP_LOGW(TAG, "Event: Getting IP address timeout");
      break;
    }
    case EVENT_SOFTAPMODE_STACONNECTED: {
      auto it = event->event_info.sta_connected;
      ESP_LOGV(TAG, "Event: AP client connected MAC=%s aid=%u",
               format_mac_addr(it.mac).c_str(), it.aid);
      break;
    }
    case EVENT_SOFTAPMODE_STADISCONNECTED: {
      auto it = event->event_info.sta_disconnected;
      ESP_LOGV(TAG, "Event: AP client disconnected MAC=%s aid=%u",
               format_mac_addr(it.mac).c_str(), it.aid);
      break;
    }
    case EVENT_SOFTAPMODE_PROBEREQRECVED: {
      auto it = event->event_info.ap_probereqrecved;
      ESP_LOGV(TAG, "Event: AP receive Probe Request MAC=%s RSSI=%d",
               format_mac_addr(it.mac).c_str(), it.rssi);
      break;
    }
    case EVENT_OPMODE_CHANGED: {
      auto it = event->event_info.opmode_changed;
      ESP_LOGV(TAG, "Event: Changed Mode old=%s new=%s",
               get_op_mode_str(it.old_opmode), get_op_mode_str(it.new_opmode));
      break;
    }
    case EVENT_SOFTAPMODE_DISTRIBUTE_STA_IP: {
      auto it = event->event_info.distribute_sta_ip;
      ESP_LOGV(TAG, "Event: AP Distribute Station IP MAC=%s IP=%s aid=%u",
               format_mac_addr(it.mac).c_str(), format_ip_addr(it.ip).c_str(), it.aid);
      break;
    }
    default:
      break;
  }

  if (event->event == EVENT_STAMODE_DISCONNECTED) {
    global_wifi_component->error_from_callback_ = true;
  }

  WiFiMockClass::_event_callback(event);
}

void WiFiComponent::wifi_register_callbacks_() {
  wifi_set_event_handler_cb(&WiFiComponent::wifi_event_callback_);
}
wl_status_t WiFiComponent::wifi_sta_status_() {
  station_status_t status = wifi_station_get_connect_status();
  switch (status) {
    case STATION_GOT_IP:
      return WL_CONNECTED;
    case STATION_NO_AP_FOUND:
      return WL_NO_SSID_AVAIL;
    case STATION_CONNECT_FAIL:
    case STATION_WRONG_PASSWORD:
      return WL_CONNECT_FAILED;
    case STATION_IDLE:
      return WL_IDLE_STATUS;
    case STATION_CONNECTING:
    default:
      return WL_DISCONNECTED;
  }
}
bool WiFiComponent::wifi_scan_start_() {
  static bool first_scan = false;

  // enable STA
  if (!this->wifi_mode_(true, {}))
    return false;

  station_status_t sta_status = wifi_station_get_connect_status();
  if (sta_status != STATION_GOT_IP && sta_status != STATION_IDLE) {
    wifi_station_disconnect();
  }

  struct scan_config config;
  memset(&config, 0, sizeof(config));
  config.ssid = nullptr;
  config.bssid = nullptr;
  config.channel = 0;
  config.show_hidden = 1;
  config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
  if (first_scan) {
    config.scan_time.active.min = 100;
    config.scan_time.active.max = 200;
  } else {
    config.scan_time.active.min = 400;
    config.scan_time.active.max = 500;
  }
  first_scan = false;
  bool ret = wifi_station_scan(&config, &WiFiComponent::s_wifi_scan_done_callback_);
  if (!ret) {
    ESP_LOGV(TAG, "wifi_station_scan failed!");
    return false;
  }

  return ret;
}
void WiFiComponent::s_wifi_scan_done_callback_(void *arg, STATUS status) {
  global_wifi_component->wifi_scan_done_callback_(arg, status);
}

void WiFiComponent::wifi_scan_done_callback_(void *arg, STATUS status) {
  this->scan_result_.clear();

  if (status != OK) {
    ESP_LOGV(TAG, "Scan failed! %d", status);
    return;
  }
  bss_info *head = reinterpret_cast<bss_info *>(arg);
  for (bss_info *it = head; it != nullptr; it = STAILQ_NEXT(it, next)) {
    WiFiScanResult res(
        {it->bssid[0], it->bssid[1], it->bssid[2], it->bssid[3], it->bssid[4], it->bssid[5]},
        std::string(reinterpret_cast<char *>(it->ssid), it->ssid_len),
        it->channel,
        it->rssi,
        it->authmode != AUTH_OPEN,
        it->is_hidden != 0
    );
    this->scan_result_.push_back(res);
  }
  this->scan_done_ = true;
}
bool WiFiComponent::wifi_ap_ip_config_(optional<ManualIP> manual_ip) {
  // enable AP
  if (!this->wifi_mode_({}, true))
    return false;

  struct ip_info info;
  if (manual_ip.has_value()) {
    info.ip.addr = static_cast<uint32_t>(manual_ip->static_ip);
    info.gw.addr = static_cast<uint32_t>(manual_ip->gateway);
    info.netmask.addr = static_cast<uint32_t>(manual_ip->subnet);
  } else {
    info.ip.addr = static_cast<uint32_t>(IPAddress(192, 168, 4, 1));
    info.gw.addr = static_cast<uint32_t>(IPAddress(192, 168, 4, 1));
    info.netmask.addr = static_cast<uint32_t>(IPAddress(255, 255, 255, 0));
  }

  if (wifi_softap_dhcps_status() == DHCP_STARTED) {
    if (!wifi_softap_dhcps_stop()) {
      ESP_LOGV(TAG, "Stopping DHCP server failed!");
    }
  }

  if (!wifi_set_ip_info(SOFTAP_IF, &info)) {
    ESP_LOGV(TAG, "Setting SoftAP info failed!");
    return false;
  }

  struct dhcps_lease lease;
  IPAddress start_address = info.ip.addr;
  start_address[3] += 99;
  lease.start_ip.addr = static_cast<uint32_t>(start_address);
  ESP_LOGV(TAG, "DHCP server IP lease start: %s", start_address.toString().c_str());
  start_address[3] += 100;
  lease.end_ip.addr = static_cast<uint32_t>(start_address);
  ESP_LOGV(TAG, "DHCP server IP lease end: %s", start_address.toString().c_str());
  if (!wifi_softap_set_dhcps_lease(&lease)) {
    ESP_LOGV(TAG, "Setting SoftAP DHCP lease failed!");
    return false;
  }

  // lease time 1440 minutes (=24 hours)
  if (!wifi_softap_set_dhcps_lease_time(1440)) {
    ESP_LOGV(TAG, "Setting SoftAP DHCP lease time failed!");
    return false;
  }

  uint8_t mode = 1;
  // bit0, 1 enables router information from ESP8266 SoftAP DHCP server.
  if (!wifi_softap_set_dhcps_offer_option(OFFER_ROUTER, &mode)) {
    ESP_LOGV(TAG, "wifi_softap_set_dhcps_offer_option failed!");
    return false;
  }

  if (!wifi_softap_dhcps_start()) {
    ESP_LOGV(TAG, "Starting SoftAP DHCPS failed!");
    return false;
  }

  return true;
}
bool WiFiComponent::wifi_start_ap_(const WiFiAP &ap) {
  // enable AP
  if (!this->wifi_mode_({}, true))
    return false;

  struct softap_config conf;
  strcpy(reinterpret_cast<char*>(conf.ssid), ap.get_ssid().c_str());
  conf.ssid_len = ap.get_ssid().size();
  conf.channel = ap.get_channel().value_or(1);
  conf.ssid_hidden = 0;
  conf.max_connection = 5;
  conf.beacon_interval = 100;

  if (ap.get_password().empty()) {
    conf.authmode = AUTH_OPEN;
    *conf.password = 0;
  } else {
    conf.authmode = AUTH_WPA2_PSK;
    strcpy(reinterpret_cast<char*>(conf.password), ap.get_password().c_str());
  }

  ETS_UART_INTR_DISABLE();
  bool ret = wifi_softap_set_config_current(&conf);
  ETS_UART_INTR_ENABLE();

  if (!ret) {
    ESP_LOGV(TAG, "wifi_softap_set_config_current failed!");
    return false;
  }

  if (!this->wifi_ap_ip_config_(ap.get_manual_ip())) {
    ESP_LOGV(TAG, "wifi_ap_ip_config_ failed!");
    return false;
  }

  return true;
}
IPAddress WiFiComponent::wifi_soft_ap_ip_() {
  struct ip_info ip;
  wifi_get_ip_info(SOFTAP_IF, &ip);
  return IPAddress(ip.ip.addr);
}
#endif
#ifdef ARDUINO_ARCH_ESP32
bool WiFiComponent::wifi_mode_(optional<bool> sta, optional<bool> ap) {
  uint8_t current_mode = WiFi.getMode();
  bool current_sta = current_mode & 0b01;
  bool current_ap = current_mode & 0b10;
  bool sta_ = sta.value_or(current_sta);
  bool ap_ = ap.value_or(current_ap);
  if (current_sta == sta_ && current_ap == ap_)
    return true;

  if (sta_ && !current_sta) {
    ESP_LOGV(TAG, "Enabling STA.");
  } else if (!sta_ && current_sta) {
    ESP_LOGV(TAG, "Disabling STA.");
  }
  if (ap_ && !current_ap) {
    ESP_LOGV(TAG, "Enabling AP.");
  } else if (!ap_ && current_ap) {
    ESP_LOGV(TAG, "Disabling AP.");
  }


  uint8_t mode = 0;
  if (sta_) mode |= 0b01;
  if (ap_) mode |= 0b10;
  bool ret = WiFi.mode(static_cast<wifi_mode_t>(mode));

  if (!ret) {
    ESP_LOGW(TAG, "Setting WiFi mode failed!");
  }

  return ret;
}
bool WiFiComponent::wifi_disable_auto_connect_() {
  WiFi.setAutoReconnect(false);
  return true;
}
bool WiFiComponent::wifi_apply_power_save_() {
  wifi_ps_type_t power_save;
  switch (this->power_save_) {
    case WIFI_POWER_SAVE_LIGHT:
      power_save = WIFI_PS_MIN_MODEM;
      break;
    case WIFI_POWER_SAVE_HIGH:
      power_save = WIFI_PS_MAX_MODEM;
      break;
    case WIFI_POWER_SAVE_NONE:
    default:
      power_save = WIFI_PS_NONE;
      break;
  }
  return esp_wifi_set_ps(power_save) == ESP_OK;
}
bool WiFiComponent::wifi_sta_ip_config_(optional<ManualIP> manual_ip) {
  // enable STA
  if (!this->wifi_mode_(true, {}))
    return false;

  tcpip_adapter_dhcp_status_t dhcp_status;
  tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA, &dhcp_status);
  if (!manual_ip.has_value()) {
    // Use DHCP client
    if (dhcp_status != TCPIP_ADAPTER_DHCP_STARTED) {
      esp_err_t err = tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
      if (err != ESP_OK) {
        ESP_LOGV(TAG, "Starting DHCP client failed! %d", err);
      }
      return err == ESP_OK;
    }
    return true;
  }

  tcpip_adapter_ip_info_t info;
  memset(&info, 0, sizeof(info));
  info.ip.addr = static_cast<uint32_t>(manual_ip->static_ip);
  info.gw.addr = static_cast<uint32_t>(manual_ip->gateway);
  info.netmask.addr = static_cast<uint32_t>(manual_ip->subnet);

  esp_err_t dhcp_stop_ret = tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
  if (dhcp_stop_ret != ESP_OK) {
    ESP_LOGV(TAG, "Stopping DHCP client failed! %d", dhcp_stop_ret);
  }

  esp_err_t wifi_set_info_ret = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &info);
  if (wifi_set_info_ret != ESP_OK) {
    ESP_LOGV(TAG, "Setting manual IP info failed! %s", esp_err_to_name(wifi_set_info_ret));
  }

  ip_addr_t dns;
  dns.type = IPADDR_TYPE_V4;
  if (!is_empty(manual_ip->dns1)) {
    dns.u_addr.ip4.addr = static_cast<uint32_t>(manual_ip->dns1);
    dns_setserver(0, &dns);
  }
  if (!is_empty(manual_ip->dns2)) {
    dns.u_addr.ip4.addr = static_cast<uint32_t>(manual_ip->dns2);
    dns_setserver(1, &dns);
  }

  return true;
}

IPAddress WiFiComponent::wifi_sta_ip_() {
  if (!this->has_sta())
    return IPAddress();
  tcpip_adapter_ip_info_t ip;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip);
  return IPAddress(ip.ip.addr);
}

bool WiFiComponent::wifi_apply_hostname_() {
  esp_err_t err = tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, get_app_name().c_str());
  if (err != ESP_OK) {
    ESP_LOGV(TAG, "Setting hostname failed: %d", err);
    return false;
  }
  return true;
}
bool WiFiComponent::wifi_sta_connect_(WiFiAP ap) {
  // enable STA
  if (!this->wifi_mode_(true, {}))
    return false;

  wifi_config_t conf;
  memset(&conf, 0, sizeof(conf));
  strcpy(reinterpret_cast<char *>(conf.sta.ssid), ap.get_ssid().c_str());
  strcpy(reinterpret_cast<char *>(conf.sta.password), ap.get_password().c_str());

  if (ap.get_bssid().has_value()) {
    conf.sta.bssid_set = 1;
    memcpy(conf.sta.bssid, ap.get_bssid()->data(), 6);
  } else {
    conf.sta.bssid_set = 0;
  }
  if (ap.get_channel().has_value()) {
    conf.sta.channel = *ap.get_channel();
  }

  esp_err_t err = esp_wifi_disconnect();
  if (err != ESP_OK) {
    ESP_LOGV(TAG, "esp_wifi_disconnect failed! %d", err);
    return false;
  }

  err = esp_wifi_set_config(WIFI_IF_STA, &conf);
  if (err != ESP_OK) {
    ESP_LOGV(TAG, "esp_wifi_set_config failed! %d", err);
  }

  if (!this->wifi_sta_ip_config_(ap.get_manual_ip())) {
    return false;
  }

  err = esp_wifi_connect();
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "esp_wifi_connect failed! %d", err);
    return false;
  }

  return true;
}
const char *get_auth_mode_str(uint8_t mode) {
  switch (mode) {
    case WIFI_AUTH_OPEN: return "OPEN";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2 PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2 PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
    default: return "UNKNOWN";
  }
}
std::string format_ip4_addr(const ip4_addr_t &ip) {
  char buf[20];
  sprintf(buf, "%u.%u.%u.%u", uint8_t(ip.addr >> 0), uint8_t(ip.addr >> 8), uint8_t(ip.addr >> 16), uint8_t(ip.addr >> 24));
  return buf;
}
const char *get_op_mode_str(uint8_t mode) {
  switch (mode) {
    case WIFI_OFF: return "OFF";
    case WIFI_STA: return "STA";
    case WIFI_AP: return "AP";
    case WIFI_AP_STA: return "AP+STA";
    default: return "UNKNOWN";
  }
}
const char *get_disconnect_reason_str(uint8_t reason) {
  switch (reason) {
    case WIFI_REASON_AUTH_EXPIRE: return "Auth Expired";
    case WIFI_REASON_AUTH_LEAVE: return "Auth Leave";
    case WIFI_REASON_ASSOC_EXPIRE: return "Association Expired";
    case WIFI_REASON_ASSOC_TOOMANY: return "Too Many Associations";
    case WIFI_REASON_NOT_AUTHED: return "Not Authenticated";
    case WIFI_REASON_NOT_ASSOCED: return "Not Associated";
    case WIFI_REASON_ASSOC_LEAVE: return "Association Leave";
    case WIFI_REASON_ASSOC_NOT_AUTHED: return "Association not Authenticated";
    case WIFI_REASON_DISASSOC_PWRCAP_BAD: return "Disassociate Power Cap Bad";
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD: return "Disassociate Supported Channel Bad";
    case WIFI_REASON_IE_INVALID: return "IE Invalid";
    case WIFI_REASON_MIC_FAILURE: return "Mic Failure";
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT: return "4-Way Handshake Timeout";
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT: return "Group Key Update Timeout";
    case WIFI_REASON_IE_IN_4WAY_DIFFERS: return "IE In 4-Way Handshake Differs";
    case WIFI_REASON_GROUP_CIPHER_INVALID: return "Group Cipher Invalid";
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID: return "Pairwise Cipher Invalid";
    case WIFI_REASON_AKMP_INVALID: return "AKMP Invalid";
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION: return "Unsupported RSN IE version";
    case WIFI_REASON_INVALID_RSN_IE_CAP: return "Invalid RSN IE Cap";
    case WIFI_REASON_802_1X_AUTH_FAILED: return "802.1x Authentication Failed";
    case WIFI_REASON_CIPHER_SUITE_REJECTED: return "Cipher Suite Rejected";
    case WIFI_REASON_BEACON_TIMEOUT: return "Beacon Timeout";
    case WIFI_REASON_NO_AP_FOUND: return "AP Not Found";
    case WIFI_REASON_AUTH_FAIL: return "Authentication Failed";
    case WIFI_REASON_ASSOC_FAIL: return "Association Failed";
    case WIFI_REASON_HANDSHAKE_TIMEOUT: return "Handshake Failed";
    case WIFI_REASON_UNSPECIFIED:
    default:
      return "Unspecified";
  }
}
void WiFiComponent::wifi_event_callback_(system_event_id_t event, system_event_info_t info) {
  switch (event) {
    case SYSTEM_EVENT_WIFI_READY: {
      ESP_LOGV(TAG, "Event: WiFi ready");
      break;
    }
    case SYSTEM_EVENT_SCAN_DONE: {
      auto it = info.scan_done;
      ESP_LOGV(TAG, "Event: WiFi Scan Done status=%u number=%u scan_id=%u",
               it.status, it.number, it.scan_id);
      break;
    }
    case SYSTEM_EVENT_STA_START: {
      ESP_LOGV(TAG, "Event: WiFi STA start");
      break;
    }
    case SYSTEM_EVENT_STA_STOP: {
      ESP_LOGV(TAG, "Event: WiFi STA stop");
      break;
    }
    case SYSTEM_EVENT_STA_CONNECTED: {
      auto it = info.connected;
      char buf[33];
      memcpy(buf, it.ssid, it.ssid_len);
      buf[it.ssid_len] = '\0';
      ESP_LOGV(TAG, "Event: Connected ssid='%s' bssid=" LOG_SECRET("%s") " channel=%u, authmode=%s",
               buf, format_mac_addr(it.bssid).c_str(), it.channel, get_auth_mode_str(it.authmode));
      break;
    }
    case SYSTEM_EVENT_STA_DISCONNECTED: {
      auto it = info.disconnected;
      char buf[33];
      memcpy(buf, it.ssid, it.ssid_len);
      buf[it.ssid_len] = '\0';
      ESP_LOGW(TAG, "Event: Disconnected ssid='%s' bssid=" LOG_SECRET("%s") " reason=%s",
               buf, format_mac_addr(it.bssid).c_str(), get_disconnect_reason_str(it.reason));
      break;
    }
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: {
      auto it = info.auth_change;
      ESP_LOGV(TAG, "Event: Authmode Change old=%s new=%s",
               get_auth_mode_str(it.old_mode), get_auth_mode_str(it.new_mode));
      break;
    }
    case SYSTEM_EVENT_STA_GOT_IP: {
      auto it = info.got_ip.ip_info;
      ESP_LOGV(TAG, "Event: Got IP static_ip=%s gateway=%s",
               format_ip4_addr(it.ip).c_str(), format_ip4_addr(it.gw).c_str());
      break;
    }
    case SYSTEM_EVENT_STA_LOST_IP: {
      ESP_LOGV(TAG, "Event: Lost IP");
      break;
    }
    case SYSTEM_EVENT_AP_START: {
      ESP_LOGV(TAG, "Event: WiFi AP start");
      break;
    }
    case SYSTEM_EVENT_AP_STOP: {
      ESP_LOGV(TAG, "Event: WiFi AP stop");
      break;
    }
    case SYSTEM_EVENT_AP_STACONNECTED: {
      auto it = info.sta_connected;
      ESP_LOGV(TAG, "Event: AP client connected MAC=%s aid=%u",
               format_mac_addr(it.mac).c_str(), it.aid);
      break;
    }
    case SYSTEM_EVENT_AP_STADISCONNECTED: {
      auto it = info.sta_disconnected;
      ESP_LOGV(TAG, "Event: AP client disconnected MAC=%s aid=%u",
               format_mac_addr(it.mac).c_str(), it.aid);
      break;
    }
    case SYSTEM_EVENT_AP_STAIPASSIGNED: {
      ESP_LOGV(TAG, "Event: AP client assigned IP");
      break;
    }
    case SYSTEM_EVENT_AP_PROBEREQRECVED: {
      auto it = info.ap_probereqrecved;
      ESP_LOGV(TAG, "Event: AP receive Probe Request MAC=%s RSSI=%d",
               format_mac_addr(it.mac).c_str(), it.rssi);
      break;
    }
    default:
      break;
  }

  if (event == SYSTEM_EVENT_STA_DISCONNECTED) {
    uint8_t reason = info.disconnected.reason;
    if (reason == WIFI_REASON_AUTH_EXPIRE || (reason >= WIFI_REASON_BEACON_TIMEOUT && reason != WIFI_REASON_AUTH_FAIL)) {
      esp_wifi_disconnect();
      this->error_from_callback_ = true;
    }
  }
  if (event == SYSTEM_EVENT_SCAN_DONE) {
    this->wifi_scan_done_callback_();
  }
}
void WiFiComponent::wifi_register_callbacks_() {
  auto f = std::bind(&WiFiComponent::wifi_event_callback_, this, std::placeholders::_1, std::placeholders::_2);
  WiFi.onEvent(f);
  WiFi.persistent(false);
}
wl_status_t WiFiComponent::wifi_sta_status_() {
  return WiFi.status();
}
bool WiFiComponent::wifi_scan_start_() {
  // enable STA
  if (!this->wifi_mode_(true, {}))
    return false;

  // need to use WiFi because of WiFiScanClass allocations :(
  int16_t err = WiFi.scanNetworks(true, true, false, 200);
  if (err != WIFI_SCAN_RUNNING) {
    ESP_LOGV(TAG, "WiFi.scanNetworks failed! %d", err);
    return false;
  }

  return true;
}
void WiFiComponent::wifi_scan_done_callback_() {
  this->scan_result_.clear();

  int16_t num = WiFi.scanComplete();
  if (num < 0)
    return;

  this->scan_result_.reserve(static_cast<unsigned int>(num));
  for (int i = 0; i < num; i++) {
    String ssid = WiFi.SSID(i);
    wifi_auth_mode_t authmode = WiFi.encryptionType(i);
    int32_t rssi = WiFi.RSSI(i);
    uint8_t *bssid = WiFi.BSSID(i);
    int32_t channel = WiFi.channel(i);

    WiFiScanResult scan(
        {bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]},
        std::string(ssid.c_str()),
        channel,
        rssi,
        authmode != WIFI_AUTH_OPEN,
        false
    );
    this->scan_result_.push_back(scan);
  }
  WiFi.scanDelete();
  this->scan_done_ = true;
}
bool WiFiComponent::wifi_ap_ip_config_(optional<ManualIP> manual_ip) {
  esp_err_t err;

  // enable AP
  if (!this->wifi_mode_({}, true))
    return false;

  tcpip_adapter_ip_info_t info;
  memset(&info, 0, sizeof(info));
  if (manual_ip.has_value()) {
    info.ip.addr = static_cast<uint32_t>(manual_ip->static_ip);
    info.gw.addr = static_cast<uint32_t>(manual_ip->gateway);
    info.netmask.addr = static_cast<uint32_t>(manual_ip->subnet);
  } else {
    info.ip.addr = static_cast<uint32_t>(IPAddress(192, 168, 4, 1));
    info.gw.addr = static_cast<uint32_t>(IPAddress(192, 168, 4, 1));
    info.netmask.addr = static_cast<uint32_t>(IPAddress(255, 255, 255, 0));
  }
  tcpip_adapter_dhcp_status_t dhcp_status;
  tcpip_adapter_dhcps_get_status(TCPIP_ADAPTER_IF_AP, &dhcp_status);
  err = tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
  if (err != ESP_OK) {
    ESP_LOGV(TAG, "tcpip_adapter_dhcps_stop failed! %d", err);
    return false;
  }

  err = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info);
  if (err != ESP_OK) {
    ESP_LOGV(TAG, "tcpip_adapter_set_ip_info failed! %d", err);
    return false;
  }

  dhcps_lease_t lease;
  lease.enable = true;
  IPAddress start_address = info.ip.addr;
  start_address[3] += 99;
  lease.start_ip.addr = static_cast<uint32_t>(start_address);
  ESP_LOGV(TAG, "DHCP server IP lease start: %s", start_address.toString().c_str());
  start_address[3] += 100;
  lease.end_ip.addr = static_cast<uint32_t>(start_address);
  ESP_LOGV(TAG, "DHCP server IP lease end: %s", start_address.toString().c_str());
  err = tcpip_adapter_dhcps_option(
      TCPIP_ADAPTER_OP_SET,
      TCPIP_ADAPTER_REQUESTED_IP_ADDRESS,
      &lease, sizeof(lease)
  );

  if (err != ESP_OK) {
    ESP_LOGV(TAG, "tcpip_adapter_dhcps_option failed! %d", err);
    return false;
  }

  err = tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);

  if (err != ESP_OK) {
    ESP_LOGV(TAG, "tcpip_adapter_dhcps_start failed! %d", err);
    return false;
  }

  return true;
}
bool WiFiComponent::wifi_start_ap_(const WiFiAP &ap) {
  // enable AP
  if (!this->wifi_mode_({}, true))
    return false;

  wifi_config_t conf;
  memset(&conf, 0, sizeof(conf));
  strcpy(reinterpret_cast<char*>(conf.ap.ssid), ap.get_ssid().c_str());
  conf.ap.channel = ap.get_channel().value_or(1);
  conf.ap.ssid_hidden = ap.get_ssid().size();
  conf.ap.max_connection = 5;
  conf.ap.beacon_interval = 100;

  if (ap.get_password().empty()) {
    conf.ap.authmode = WIFI_AUTH_OPEN;
    *conf.ap.password = 0;
  } else {
    conf.ap.authmode = WIFI_AUTH_WPA2_PSK;
    strcpy(reinterpret_cast<char*>(conf.ap.password), ap.get_password().c_str());
  }

  esp_err_t err = esp_wifi_set_config(WIFI_IF_AP, &conf);
  if (err != ESP_OK) {
    ESP_LOGV(TAG, "esp_wifi_set_config failed! %d", err);
    return false;
  }

  yield();

  if (!this->wifi_ap_ip_config_(ap.get_manual_ip())) {
    ESP_LOGV(TAG, "wifi_ap_ip_config_ failed!");
    return false;
  }

  return true;
}
IPAddress WiFiComponent::wifi_soft_ap_ip_() {
  tcpip_adapter_ip_info_t ip;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip);
  return IPAddress(ip.ip.addr);
}
#endif

void WiFiAP::set_ssid(const std::string &ssid) { this->ssid_ = ssid; }
void WiFiAP::set_bssid(bssid_t bssid) { this->bssid_ = bssid; }
void WiFiAP::set_bssid(optional<bssid_t> bssid) { this->bssid_ = bssid; }
void WiFiAP::set_password(const std::string &password) { this->password_ = password; }
void WiFiAP::set_channel(optional<uint8_t> channel) { this->channel_ = channel; }
void WiFiAP::set_manual_ip(optional<ManualIP> manual_ip) { this->manual_ip_ = manual_ip; }
void WiFiAP::set_hidden(bool hidden) { this->hidden_ = hidden; }
const std::string &WiFiAP::get_ssid() const { return this->ssid_; }
const optional<bssid_t> &WiFiAP::get_bssid() const { return this->bssid_; }
const std::string &WiFiAP::get_password() const { return this->password_; }
const optional<uint8_t> &WiFiAP::get_channel() const { return this->channel_; }
const optional<ManualIP> &WiFiAP::get_manual_ip() const { return this->manual_ip_; }
bool WiFiAP::get_hidden() const { return this->hidden_; }

WiFiScanResult::WiFiScanResult(const bssid_t &bssid,
                               const std::string &ssid,
                               uint8_t channel,
                               int8_t rssi,
                               bool with_auth,
                               bool is_hidden)
    : bssid_(bssid),
      ssid_(ssid),
      channel_(channel),
      rssi_(rssi),
      with_auth_(with_auth),
      is_hidden_(is_hidden) {

}
bool WiFiScanResult::matches(const WiFiAP &ap) {
  if (this->is_hidden_ || this->ssid_.empty()) {
    // SSID is hidden
    if (!ap.get_hidden())
      return false;
  } else {
    if (ap.get_ssid() != this->ssid_)
      return false;
  }
  if (ap.get_bssid().has_value() && *ap.get_bssid() != this->bssid_)
    return false;
  if (ap.get_password().empty() == this->with_auth_)
    return false;
  if (ap.get_channel().has_value()) {
    if (*ap.get_channel() != this->channel_)
      return false;
  }
  return true;
}
bool WiFiScanResult::get_matches() const { return this->matches_; }
void WiFiScanResult::set_matches(bool matches) { this->matches_ = matches; }
const bssid_t &WiFiScanResult::get_bssid() const { return this->bssid_; }
const std::string &WiFiScanResult::get_ssid() const { return this->ssid_; }
uint8_t WiFiScanResult::get_channel() const { return this->channel_; }
int8_t WiFiScanResult::get_rssi() const { return this->rssi_; }
bool WiFiScanResult::get_with_auth() const { return this->with_auth_; }
bool WiFiScanResult::get_is_hidden() const { return this->is_hidden_; }

WiFiComponent *global_wifi_component;

ESPHOME_NAMESPACE_END
