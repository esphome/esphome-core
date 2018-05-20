//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_WIFI_COMPONENT_H
#define ESPHOMELIB_WIFI_COMPONENT_H

#include <string>
#include <IPAddress.h>

#ifdef ARDUINO_ARCH_ESP32
  #include <esp_wifi.h>
  #include <WiFiType.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include <ESP8266WiFiType.h>
#endif

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

ESPHOMELIB_NAMESPACE_BEGIN

/// Struct for setting static IPs in WiFiComponent.
struct ManualIP {
  IPAddress static_ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1; ///< The first DNS server. 0.0.0.0 for default.
  IPAddress dns2; ///< The second DNS server. 0.0.0.0 for default.
};

/// This component is responsible for managing the ESP WiFi interface.
class WiFiComponent : public Component {
 public:
  /// Construct a WiFiComponent.
  WiFiComponent();

  /// Setup the STA (client) mode. The parameters define which station to connect to.
  void set_sta(const std::string &ssid, const std::string &password);

  /// Manually set a static IP for this WiFi interface.
  void set_sta_manual_ip(ManualIP manual_ip);

  /** Setup an Access Point that should be created if no connection to a station can be made.
   *
   * This can also be used without set_sta(). Then the AP will always be active.
   *
   * If both STA and AP are defined, then both will be enabled at startup, but if a connection to a station
   * can be made, the AP will be turned off again.
   */
  void set_ap(const std::string &ssid, const std::string &password = "", uint8_t channel = 1);

  /// Manually set the manual IP for the AP.
  void set_ap_manual_ip(ManualIP manual_ip);

  /// Set the advertised hostname, defaults to the App name.
  void set_hostname(std::string &&hostname);
  const std::string &get_hostname();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup WiFi interface.
  void setup() override;
  /// WIFI setup_priority.
  float get_setup_priority() const override;
  float get_loop_priority() const override;

  /// Reconnect WiFi if required.
  void loop() override;

  bool has_sta() const;
  bool has_ap() const;

 protected:
  void setup_sta_config(bool show_config = true);

  void setup_ap_config();

#ifdef ARDUINO_ARCH_ESP32
  /// Used for logging WiFi events.
  static void on_wifi_event(WiFiEvent_t event);
#endif

  /// Waits for the WiFi class to return a connected state.
  void wait_for_sta();

  void sta_connected();

  std::string hostname_;

  bool sta_on_;
  std::string sta_ssid_;
  std::string sta_password_;
  optional<ManualIP> sta_manual_ip_;

  bool ap_on_;
  std::string ap_ssid_;
  std::string ap_password_;
  uint8_t ap_channel_;
  optional<ManualIP> ap_manual_ip_;
};

extern WiFiComponent *global_wifi_component;

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_WIFI_COMPONENT_H
