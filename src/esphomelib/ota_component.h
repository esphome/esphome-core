//
// Created by Otto Winter on 03.12.17.
//

#ifndef ESPHOMELIB_OTA_COMPONENT_H
#define ESPHOMELIB_OTA_COMPONENT_H

#include <WiFiServer.h>
#include "component.h"
namespace esphomelib {

/// OTAComponent provides a simple way to integrate Over-the-Air updates into your app using ArduinoOTA.
class OTAComponent : public Component {
 public:
  /** Construct an OTAComponent. Defaults to no authentication.
   *
   * @param port The port ArduinoOTA will listen on.
   * @param hostname The hostname ArduinoOTA will advertise.
   */
  explicit OTAComponent(uint16_t port = 3232, std::string hostname = "");

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

  uint16_t get_port() const;
  void set_port(uint16_t port);
  const std::string &get_hostname() const;
  /// Empty for default hostname.
  void set_hostname(const std::string &hostname);

  void setup() override;
  float get_setup_priority() const override;
  void loop() override;

 private:
  enum { OPEN, PLAINTEXT, HASH } auth_type_;

  union {
    std::string password_;
    std::string password_hash_;
  };

  uint16_t port_;
  std::string hostname_;
  WiFiServer *server_;
};

} // namespace esphomelib

#endif //ESPHOMELIB_OTA_COMPONENT_H
