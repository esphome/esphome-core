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

enum WiFiComponentState {
  /** Nothing has been initialized yet. Internal AP, if configured, is disabled at this point.
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_AP (when AP-only mode)
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING (when in STA-only mode)
   *   - WIFI_COMPONENT_STATE_AP_STA_CONNECTING (when in AP+STA mode)
   */
  WIFI_COMPONENT_STATE_OFF = 0,
  /** WiFi is in STA-only mode and currently connecting to an AP.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_OFF (when scan complete and attempting connection)
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_STA_CONNECTED (when connecting was successful)
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING (when connecting fails)
   */
  WIFI_COMPONENT_STATE_STA_CONNECTING,
  // Any state below here is a valid state for continuing
  /** WiFi is in AP-only mode and internal AP is already enabled.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_OFF (on boot)
   */
  WIFI_COMPONENT_STATE_AP,
  /** WiFi is in STA-only mode and successfully connected.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING (when connecting was successful)
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING (when connection is lost)
   */
  WIFI_COMPONENT_STATE_STA_CONNECTED,
  /** WiFi is in AP+sta mode and currently connecting to an AP. Internal AP is enabled at this point.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_AP_STA_SCANNING (when scan complete and attempting connection)
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_AP_STA_CONNECTED (when connecting was successful)
   *   - WIFI_COMPONENT_STATE_AP_STA_CONNECTING (when connecting fails)
   */
  WIFI_COMPONENT_STATE_AP_STA_CONNECTING,
  /** WiFi is in AP+STA mode and successfully connected. Internal AP is disabled at this point.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_AP_STA_CONNECTING (when connecting was successful)
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_AP_STA_CONNECTING (when connection is lost, internal AP is enabled again)
   */
  WIFI_COMPONENT_STATE_AP_STA_CONNECTED,
};

/// Struct for setting static IPs in WiFiComponent.
struct ManualIP {
  IPAddress static_ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1; ///< The first DNS server. 0.0.0.0 for default.
  IPAddress dns2; ///< The second DNS server. 0.0.0.0 for default.
};

struct WiFiAp {
  std::string ssid;
  std::string password;
  int8_t channel; ///< only for AP-mode
  optional<ManualIP> manual_ip;
};

/// This component is responsible for managing the ESP WiFi interface.
class WiFiComponent : public Component {
 public:
  /// Construct a WiFiComponent.
  WiFiComponent();

  /// Setup the STA (client) mode. The parameters define which station to connect to.
  void set_sta(const WiFiAp &ap);

  /** Setup an Access Point that should be created if no connection to a station can be made.
   *
   * This can also be used without set_sta(). Then the AP will always be active.
   *
   * If both STA and AP are defined, then both will be enabled at startup, but if a connection to a station
   * can be made, the AP will be turned off again.
   */
  void set_ap(const WiFiAp &ap);

  /// Set the advertised hostname, defaults to the App name.
  void set_hostname(std::string &&hostname);
  const std::string &get_hostname();

  void start_connecting();

  void check_connecting_finished();

  void retry_connect();

  bool can_proceed() override;

  void set_reboot_timeout(uint32_t reboot_timeout);

  bool is_connected();

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
  void setup_ap_config();

#ifdef ARDUINO_ARCH_ESP32
  /// Used for logging WiFi events.
  static void on_wifi_event(WiFiEvent_t event);
#endif

  std::string hostname_;

  WiFiAp sta_;

  WiFiAp ap_;
  WiFiComponentState state_{WIFI_COMPONENT_STATE_OFF};
  uint32_t action_started_;
  uint8_t num_retried_{0};
  uint32_t last_connected_{0};
  uint32_t reboot_timeout_{60000};
};

extern WiFiComponent *global_wifi_component;

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_WIFI_COMPONENT_H
