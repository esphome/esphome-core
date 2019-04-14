#ifndef ESPHOME_OTA_COMPONENT_H
#define ESPHOME_OTA_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_OTA

#include "esphome/component.h"
#include "esphome/esppreferences.h"
#include <WiFiServer.h>
#include <WiFiClient.h>

#ifdef ARDUINO_ARCH_ESP32
#define OTA_DEFAULT_PORT 3232
#endif
#ifdef ARDUINO_ARCH_ESP8266
#define OTA_DEFAULT_PORT 8266
#endif

ESPHOME_NAMESPACE_BEGIN

enum OTAResponseTypes {
  OTA_RESPONSE_OK = 0,
  OTA_RESPONSE_REQUEST_AUTH = 1,

  OTA_RESPONSE_HEADER_OK = 64,
  OTA_RESPONSE_AUTH_OK = 65,
  OTA_RESPONSE_UPDATE_PREPARE_OK = 66,
  OTA_RESPONSE_BIN_MD5_OK = 67,
  OTA_RESPONSE_RECEIVE_OK = 68,
  OTA_RESPONSE_UPDATE_END_OK = 69,

  OTA_RESPONSE_ERROR_MAGIC = 128,
  OTA_RESPONSE_ERROR_UPDATE_PREPARE = 129,
  OTA_RESPONSE_ERROR_AUTH_INVALID = 130,
  OTA_RESPONSE_ERROR_WRITING_FLASH = 131,
  OTA_RESPONSE_ERROR_UPDATE_END = 132,
  OTA_RESPONSE_ERROR_INVALID_BOOTSTRAPPING = 133,
  OTA_RESPONSE_ERROR_WRONG_CURRENT_FLASH_CONFIG = 134,
  OTA_RESPONSE_ERROR_WRONG_NEW_FLASH_CONFIG = 135,
  OTA_RESPONSE_ERROR_ESP8266_NOT_ENOUGH_SPACE = 136,
  OTA_RESPONSE_ERROR_ESP32_NOT_ENOUGH_SPACE = 137,
  OTA_RESPONSE_ERROR_UNKNOWN = 255,
};

extern uint8_t OTA_VERSION_1_0;

/// OTAComponent provides a simple way to integrate Over-the-Air updates into your app using ArduinoOTA.
class OTAComponent : public Component {
 public:
  /** Construct an OTAComponent. Defaults to no authentication.
   *
   * @param port The port ArduinoOTA will listen on.
   */
  explicit OTAComponent(uint16_t port = OTA_DEFAULT_PORT);

  /** Set a plaintext password that OTA will use for authentication.
   *
   * Warning: This password will be stored in plaintext in the ROM and can be read
   * by intruders.
   *
   * @param password The plaintext password.
   */
  void set_auth_password(const std::string &password);

  /// Manually set the port OTA should listen on.
  void set_port(uint16_t port);

  /** Start OTA safe mode. When called at startup, this method will automatically detect boot loops.
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
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

  uint16_t get_port() const;

  void clean_rtc();

 protected:
  void write_rtc_(uint32_t val);
  uint32_t read_rtc_();

  void handle_();
  size_t wait_receive_(uint8_t *buf, size_t bytes, bool check_disconnected = true);

  std::string password_;

  uint16_t port_;

  WiFiServer *server_{nullptr};
  WiFiClient client_{};

  bool has_safe_mode_{false};              ///< stores whether safe mode can be enabled.
  uint32_t safe_mode_start_time_;          ///< stores when safe mode was enabled.
  uint32_t safe_mode_enable_time_{60000};  ///< The time safe mode should be on for.
  uint32_t safe_mode_rtc_value_;
  uint8_t safe_mode_num_attempts_;
  uint8_t at_ota_progress_message_{0};  ///< store OTA progress message index so that we don't spam logs
  ESPPreferenceObject rtc_;
};

ESPHOME_NAMESPACE_END

#endif  // USE_OTA

#endif  // ESPHOME_OTA_COMPONENT_H
