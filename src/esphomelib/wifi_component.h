#ifndef ESPHOMELIB_WIFI_COMPONENT_H
#define ESPHOMELIB_WIFI_COMPONENT_H

#include <string>
#include <IPAddress.h>

#ifdef ARDUINO_ARCH_ESP32
  #include <esp_wifi.h>
  #include <WiFiType.h>
  #include <WiFi.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include <ESP8266WiFiType.h>
  #include <ESP8266WiFi.h>
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
  WIFI_COMPONENT_STATE_COOLDOWN,
  /** WiFi is in STA-only mode and currently scanning for APs.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_OFF
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING (when connecting fails)
   */
   WIFI_COMPONENT_STATE_STA_SCANNING,
  /** WiFi is in STA(+AP) mode and currently connecting to an AP.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_OFF (when connecting using Probe Request)
   *   - WIFI_COMPONENT_STATE_STA_SCANNING (when connecting from Search Result)
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_STA_CONNECTED (when connecting was successful)
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING (when connecting fails)
   */
  WIFI_COMPONENT_STATE_STA_CONNECTING,
  /** WiFi is in STA(+AP) mode and successfully connected.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_STA_CONNECTING (when connecting was successful)
   *
   * State can transition to:
   *   - WIFI_COMPONENT_STATE_OFF (when connection is lost)
   *   - WIFI_COMPONENT_STATE_STA_SCANNING (when connection is lost)
   */
      WIFI_COMPONENT_STATE_STA_CONNECTED,
  // Any state below here is a valid state for continuing
  /** WiFi is in AP-only mode and internal AP is already enabled.
   *
   * State can transition here from:
   *   - WIFI_COMPONENT_STATE_OFF (on boot)
   */
  WIFI_COMPONENT_STATE_AP,
};

/// Struct for setting static IPs in WiFiComponent.
struct ManualIP {
  IPAddress static_ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1; ///< The first DNS server. 0.0.0.0 for default.
  IPAddress dns2; ///< The second DNS server. 0.0.0.0 for default.
};

using bssid_t = std::array<uint8_t, 6>;

class WiFiAP {
 public:
  void set_ssid(const std::string &ssid);
  void set_bssid(optional<bssid_t> bssid);
  void set_password(const std::string &password);
  void set_channel(optional<uint8_t> channel);
  void set_manual_ip(optional<ManualIP> manual_ip);
  const std::string &get_ssid() const;
  const optional<bssid_t> &get_bssid() const;
  const std::string &get_password() const;
  const optional<uint8_t> &get_channel() const;
  const optional<ManualIP> &get_manual_ip() const;
 protected:
  std::string ssid_;
  optional<bssid_t> bssid_;
  std::string password_;
  optional<uint8_t> channel_;
  optional<ManualIP> manual_ip_;
};

class WiFiScanResult {
 public:
  WiFiScanResult(const bssid_t &bssid,
                 const std::string &ssid,
                 uint8_t channel,
                 int8_t rssi,
                 bool with_auth,
                 bool is_hidden);

  bool matches(const WiFiAP &ap);

  bool get_matches() const;
  void set_matches(bool matches);
  const bssid_t &get_bssid() const;
  const std::string &get_ssid() const;
  uint8_t get_channel() const;
  int8_t get_rssi() const;
  bool get_with_auth() const;
  bool get_is_hidden() const;
 protected:
  bool matches_{false};
  bssid_t bssid_;
  std::string ssid_;
  uint8_t channel_;
  int8_t rssi_;
  bool with_auth_;
  bool is_hidden_;

};

enum WiFiPowerSaveMode {
  WIFI_POWER_SAVE_NONE = 0,
  WIFI_POWER_SAVE_LIGHT,
  WIFI_POWER_SAVE_HIGH,
};

/// This component is responsible for managing the ESP WiFi interface.
class WiFiComponent : public Component {
 public:
  /// Construct a WiFiComponent.
  WiFiComponent();

  void add_sta(const WiFiAP &ap);

  /** Setup an Access Point that should be created if no connection to a station can be made.
   *
   * This can also be used without set_sta(). Then the AP will always be active.
   *
   * If both STA and AP are defined, then both will be enabled at startup, but if a connection to a station
   * can be made, the AP will be turned off again.
   */
  void set_ap(const WiFiAP &ap);

  /// Set the advertised hostname, defaults to the App name.
  void set_hostname(std::string &&hostname);
  const std::string &get_hostname();

  void start_scanning();
  void check_scanning_finished();
  void start_connecting(const WiFiAP &ap);

  void check_connecting_finished();

  void retry_connect();

  bool can_proceed() override;

  bool ready_for_ota();

  void set_reboot_timeout(uint32_t reboot_timeout);

  bool is_connected();

  /** Set the power save option for the WiFi interface.
   *
   * Options are:
   *  * WIFI_POWER_SAVE_NONE (default, least power saving)
   *  * WIFI_POWER_SAVE_LIGHT
   *  * WIFI_POWER_SAVE_HIGH (try to save as much power as possible)
   *
   * Note that this can affect WiFi performance, for example a higher power saving option
   * can increase the amount of random disconnects from the WiFi router.
   *
   * @param power_save The power save mode.
   */
  void set_power_save_mode(WiFiPowerSaveMode power_save);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup WiFi interface.
  void setup() override;
  void dump_config() override;
  /// WIFI setup_priority.
  float get_setup_priority() const override;
  float get_loop_priority() const override;

  /// Reconnect WiFi if required.
  void loop() override;

  bool has_sta() const;
  bool has_ap() const;

 protected:
  void setup_ap_config();
  void print_connect_params_();

  bool wifi_mode_(optional<bool> sta, optional<bool> ap);
  bool wifi_disable_auto_connect_();
  bool wifi_apply_power_save_();
  bool wifi_sta_ip_config_(optional<ManualIP> manual_ip);
  bool wifi_apply_hostname_();
  bool wifi_sta_connect_(WiFiAP ap);
  void wifi_register_callbacks_();
  wl_status_t wifi_sta_status_();
  bool wifi_scan_start_();
  bool wifi_ap_ip_config_(optional<ManualIP> manual_ip);
  bool wifi_start_ap_(const WiFiAP &ap);
  IPAddress wifi_soft_ap_ip_();

#ifdef ARDUINO_ARCH_ESP8266
  static void wifi_event_callback_(System_Event_t *arg);
  void wifi_scan_done_callback_(void *arg, STATUS status);
  static void s_wifi_scan_done_callback_(void *arg, STATUS status);
#endif

#ifdef ARDUINO_ARCH_ESP32
  void wifi_event_callback_(system_event_id_t event, system_event_info_t info);
  void wifi_scan_done_callback_();
#endif

  std::string hostname_;

  std::vector<WiFiAP> sta_;

  WiFiAP ap_;
  WiFiComponentState state_{WIFI_COMPONENT_STATE_OFF};
  uint32_t action_started_;
  uint8_t num_retried_{0};
  uint32_t last_connected_{0};
  uint32_t reboot_timeout_{300000};
  WiFiPowerSaveMode power_save_{WIFI_POWER_SAVE_NONE};
  bool error_from_callback_{false};
  std::vector<WiFiScanResult> scan_result_;
  bool scan_done_{false};
  bool ap_setup_{false};
};

extern WiFiComponent *global_wifi_component;

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_WIFI_COMPONENT_H
