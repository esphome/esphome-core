//
// Created by Otto Winter on 03.12.17.
//

#ifndef ESPHOMELIB_OTA_COMPONENT_H
#define ESPHOMELIB_OTA_COMPONENT_H

#include <WiFiServer.h>

#include "esphomelib/component.h"

#ifdef ARDUINO_ARCH_ESP32
  const uint16_t OTA_DEFAULT_PORT = 3232;
#else
  const uint16_t OTA_DEFAULT_PORT = 8266;
#endif

namespace esphomelib {

/// OTAComponent provides a simple way to integrate Over-the-Air updates into your app using ArduinoOTA.
class OTAComponent : public Component {
 public:
  /** Construct an OTAComponent. Defaults to no authentication.
   *
   * @param port The port ArduinoOTA will listen on.
   * @param hostname The hostname ArduinoOTA will advertise.
   */
  explicit OTAComponent(uint16_t port = OTA_DEFAULT_PORT, std::string hostname = "");

  /// Set ArduinoOTA to accept updates without authentication.
  void set_auth_open();

  /** Set a plaintext password that ArduinoOTA will use for authentication.
   *
   * Note: theoretically this password can be read from ROM by an intruder.
   *
   * @param password The plaintext password.
   */
  void set_auth_plaintext_password(const std::string &password);

  /** Set a MD5 password hash that ArduinoOTA will use for authentication.
   *
   * @param hash The MD5 hash of the password.
   */
  void set_auth_password_hash(const std::string &hash);

  /// Manually set the port OTA should listen on.
  void set_port(uint16_t port);

  /// Set the hostname advertised with mDNS. Empty for default hostname.
  void set_hostname(const std::string &hostname);

  /** Star OTA safe mode. When called at startup, this method will automatically detect boot loops.
   *
   * If a boot loop is detected (by `num_attempts` boots which each lasted less than `enable_time`),
   * this method will block startup and enable just WiFi and OTA so that users can flash a new image.
   *
   * When in boot loop safe mode, if no OTA attempt is made within `enable_time` milliseconds, the device
   * is restarted. If the device has stayed on for more than `enable_time` milliseconds, the boot is considered
   * successful and the num_attempts counter is reset.
   *
   * @param num_attempts The number of failed boot attempts until which time safe mode should be enabled.
   * @param enable_time The time in ms safe mode should be on before restarting.
   */
  void start_safe_mode(uint8_t num_attempts = 10, uint32_t enable_time = 120000);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  void setup() override;
  float get_setup_priority() const override;
  void loop() override;

  const std::string &get_hostname() const;

  uint16_t get_port() const;

 protected:
  enum { OPEN, PLAINTEXT, HASH } auth_type_;

  union {
    std::string password_;
    std::string password_hash_;
  };

  uint16_t port_;
  std::string hostname_;
  WiFiServer *server_;
  bool ota_triggered_{false}; ///< stores whether OTA is currently active.
  bool has_safe_mode_{false}; ///< stores whether safe mode can be enabled.
  uint32_t safe_mode_start_time_; ///<stores when safe mode was enabled.
  uint32_t safe_mode_enable_time_{60000}; ///< The time safe mode should be on for.
  uint8_t at_ota_progress_message_{0}; ///< store OTA progress message index so that we don't spam logs
};

} // namespace esphomelib

#endif //ESPHOMELIB_OTA_COMPONENT_H
