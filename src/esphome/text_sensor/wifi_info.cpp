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

void SSIDWiFiInfo::loop() {
  const char *ssid = WiFi.SSID().c_str();
  if (this->last_ssid_ != ssid) {
    this->last_ssid_ = ssid;
    this->publish_state(ssid);
  }
}

void BSSIDWiFiInfo::loop() {
  const char *bssid = WiFi.BSSIDstr().c_str();
  if (this->last_bssid_ != bssid) {
    this->last_bssid_ = bssid;
    this->publish_state(bssid);
  }
}

} // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif //USE_WIFI_INFO_TEXT_SENSOR
