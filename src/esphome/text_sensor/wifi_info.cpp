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

void SSIDWiFiInfo::loop() {
  String ssid = WiFi.SSID();
  if (this->last_ssid_ != ssid.c_str()) {
    this->last_ssid_ = std::string(ssid.c_str());
    this->publish_state(this->last_ssid_);
  }
}
SSIDWiFiInfo::SSIDWiFiInfo(const std::string &name) : TextSensor(name) {}

void BSSIDWiFiInfo::loop() {
  String bssid = WiFi.BSSIDstr();
  if (this->last_bssid_ != bssid.c_str()) {
    this->last_bssid_ = bssid.c_str();
    this->publish_state(this->last_bssid_);
  }
}
BSSIDWiFiInfo::BSSIDWiFiInfo(const std::string &name) : TextSensor(name) {}

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_WIFI_INFO_TEXT_SENSOR
