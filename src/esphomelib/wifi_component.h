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

namespace esphomelib {

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
  /** Construct a WiFiComponent.
   *
   * @param ssid The station ssid.
   * @param password The station password, leave empty for an open network.
   * @param hostname The hostname of this device, leave empty for standard one.
   */
  WiFiComponent(std::string ssid, std::string password, std::string hostname);

  /// Set the hostname, automatically sanitizes it.
  void set_hostname(const std::string &hostname);

  /// Manually set a static IP for this WiFi interface.
  void set_manual_ip(Optional<ManualIP> manual_ip);

  void set_ssid(const std::string &ssid);

  void set_password(const std::string &password);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  const std::string &get_hostname() const;
  const Optional<ManualIP> &get_manual_ip() const;
  const std::string &get_ssid() const;
  const std::string &get_password() const;

  /// Setup WiFi interface.
  void setup() override;
  /// WIFI setup_priority.
  float get_setup_priority() const override;
  /// Reconnect WiFi if required.
  void loop() override;

 protected:
  /// Used for logging WiFi events.
  static void on_wifi_event(WiFiEvent_t event);

  /// Waits for the WiFi class to return a connected state.
  void wait_for_connection();

  std::string ssid_;
  std::string password_;
  std::string hostname_;
  Optional<ManualIP> manual_ip_;
};

} // namespace esphomelib

#endif //ESPHOMELIB_WIFI_COMPONENT_H
