#ifndef ESPHOME_CORE_TEXT_SENSOR_WIFI_INFO_H
#define ESPHOME_CORE_TEXT_SENSOR_WIFI_INFO_H

#include "esphome/defines.h"

#ifdef USE_WIFI_INFO_TEXT_SENSOR

#include "esphome/component.h"
#include "esphome/wifi_component.h"
#include "esphome/text_sensor/text_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class IPAddressWiFiInfo : public Component, public TextSensor {
 public:
  IPAddressWiFiInfo(const std::string &name);
  void loop() override;
  float get_setup_priority() const override;

 protected:
  IPAddress last_ip_;
};

class SSIDWiFiInfo : public Component, public TextSensor {
 public:
  SSIDWiFiInfo(const std::string &name);
  void loop() override;
  float get_setup_priority() const override;

 protected:
  std::string last_ssid_;
};

class BSSIDWiFiInfo : public Component, public TextSensor {
 public:
  BSSIDWiFiInfo(const std::string &name);
  void loop() override;
  float get_setup_priority() const override;

 protected:
  bssid_t last_bssid_;
};

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_WIFI_INFO_TEXT_SENSOR

#endif  // ESPHOME_CORE_TEXT_SENSOR_WIFI_INFO_H
