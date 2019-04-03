#include "esphome/defines.h"

#ifdef USE_WIFI_INFO_TEXT_SENSOR

#include "esphome/text_sensor/wifi_info.h"
#include "esphome/wifi_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.wifi_info";

void IPAddressWiFiInfo::loop() {
  IPAddress ip = WiFi.localIP();
  if (ip != this->last_ip_) {
    this->last_ip_ = ip;
    this->publish_state(ip.toString().c_str());
  }
}
IPAddressWiFiInfo::IPAddressWiFiInfo(const std::string &name) : TextSensor(name) {}
float IPAddressWiFiInfo::get_setup_priority() const { return setup_priority::WIFI - 1.0f; }

void SSIDWiFiInfo::loop() {
  String ssid = WiFi.SSID();
  if (this->last_ssid_ != ssid.c_str()) {
    this->last_ssid_ = std::string(ssid.c_str());
    this->publish_state(this->last_ssid_);
  }
}
SSIDWiFiInfo::SSIDWiFiInfo(const std::string &name) : TextSensor(name) {}
float SSIDWiFiInfo::get_setup_priority() const { return setup_priority::WIFI - 1.0f; }

void BSSIDWiFiInfo::loop() {
  uint8_t *bssid = WiFi.BSSID();
  if (memcmp(bssid, this->last_bssid_.data(), 6) != 0) {
    std::copy(bssid, bssid + 6, this->last_bssid_.data());
    char buf[30];
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    this->publish_state(buf);
  }
}
BSSIDWiFiInfo::BSSIDWiFiInfo(const std::string &name) : TextSensor(name) {}
float BSSIDWiFiInfo::get_setup_priority() const { return setup_priority::WIFI - 1.0f; }

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_WIFI_INFO_TEXT_SENSOR
