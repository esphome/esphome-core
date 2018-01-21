//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_LOG_COMPONENT_H
#define ESPHOMELIB_LOG_COMPONENT_H

#include <cstdarg>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "component.h"
#include <esp_log.h>
#include <cassert>
#include "esp32-hal-log.h"

namespace esphomelib {

/** LogComponent - A simple component that enables logging to Serial and MQTT via ESP_LOG* macros.
 *
 * This component should optimally be setup very early because only after its setup log messages are actually sent.
 * To do this, simply call pre_setup() as early as possible.
 */
class LogComponent : public Component {
 public:
  /** Construct the LogComponent.
   *
   * @param baud_rate The baud_rate for the serial interface. 0 to disable UART logging.
   * @param mqtt_topic The topic to push MQTT logs to. Empty string to disable MQTT logging.
   * @param tx_buffer_size The buffer size (in bytes) used for constructing log messages.
   */
  explicit LogComponent(uint32_t baud_rate = 11520, std::string mqtt_topic = "", size_t tx_buffer_size = 512);

  /// Set up this component.
  void pre_setup();

  uint32_t get_baud_rate() const;
  void set_baud_rate(uint32_t baud_rate);
  const std::string &get_mqtt_topic() const;
  void set_mqtt_topic(const std::string &mqtt_topic);
  size_t get_tx_buffer_size() const;
  void set_tx_buffer_size(size_t tx_buffer_size);
  /// Set the global log level.
  void set_global_log_level(esp_log_level_t log_level);
  /// Set the log level of the specified tag.
  void set_log_level(const std::string &tag, esp_log_level_t log_level);

 private:
  static int log_vprintf_(const char *format, va_list args);

  uint32_t baud_rate_;
  std::vector<char> tx_buffer_;
  std::string mqtt_topic_;
};

void __assert_func(const char *file, int lineno, const char *func, const char *exp);

extern LogComponent *global_log_component;

} // namespace esphomelib

#endif //ESPHOMELIB_LOG_COMPONENT_H
