//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_WIFI_COMPONENT_H
#define ESPHOMELIB_WIFI_COMPONENT_H

#include "component.h"
#include "helpers.h"
#include <string>
#include <IPAddress.h>
#include <WiFiType.h>
#include <esp_event.h>

namespace esphomelib {

/// Struct for setting static IPs in WiFiComponent.
struct ManualIP {
  IPAddress static_ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1; ///< The first DNS server. 0.0.0.0 for default.
  IPAddress dns2; ///< The second DNS server. 0.0.0.0 for default.
};

/// WiFiComponent - This component is responsible for managing the ESP32 WiFi interface.
class WiFiComponent : public Component {
 public:
  /** Construct a WiFiComponent.
   *
   * @param ssid The station ssid.
   * @param password The station password, leave empty for an open network.
   * @param hostname The hostname of this device, leave empty for standard one.
   */
  WiFiComponent(std::string ssid, std::string password, std::string hostname);

  void set_hostname(std::string hostname);
  const std::string &get_hostname() const;

  /// Manually set a static IP for this WiFi interface.
  void set_manual_ip(Optional<ManualIP> manual_ip);
  const Optional<ManualIP> &get_manual_ip() const;

  void set_ssid(const std::string &ssid);
  const std::string &get_ssid() const;

  void set_password(const std::string &password);
  const std::string &get_password() const;

  void setup() override;
  float get_setup_priority() const override;
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
