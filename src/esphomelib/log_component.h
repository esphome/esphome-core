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
#include <cassert>
#include <unordered_map>

#include "esphomelib/component.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

namespace esphomelib {

/**  A simple component that enables logging to Serial and MQTT via ESP_LOG* macros.
 *
 * This component should optimally be setup very early because only after its setup log messages are actually sent.
 * To do this, simply call pre_setup() as early as possible.
 */
class LogComponent : public Component {
 public:
  /** Construct the LogComponent.
   *
   * @param baud_rate The baud_rate for the serial interface. 0 to disable UART logging.
   * @param tx_buffer_size The buffer size (in bytes) used for constructing log messages.
   */
  explicit LogComponent(uint32_t baud_rate = 11520, size_t tx_buffer_size = 512);

  /// Manually set the baud rate for serial, set to 0 to disable.
  void set_baud_rate(uint32_t baud_rate);

  /// Set a custom MQTT logging topic. Set to "" for default behavior. This will also enable logging.
  void set_custom_logging_topic(const std::string &custom_logging_topic);

  /// Enable/Disable logging to MQTT
  void set_mqtt_logging_enabled(bool mqtt_logging_enabled);

  /// Set the buffer size that's used for constructing log messages. Log messages longer than this will be truncated.
  void set_tx_buffer_size(size_t tx_buffer_size);

  /// Set the global log level. Note: Use the ESPHOMELIB_LOG_LEVEL define to also remove the logs from the build.
  void set_global_log_level(ESPLogLevel log_level);

  /// Set the log level of the specified tag.
  void set_log_level(const std::string &tag, ESPLogLevel log_level);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Set up this component.
  void pre_setup();
  uint32_t get_baud_rate() const;

  /// Return the logging topic, opting for the default if it hasn't been customized.
  std::string get_logging_topic();

  /// Whether logging to MQTT is enabled.
  bool is_mqtt_logging_enabled() const;

  size_t get_tx_buffer_size() const;

  int log_vprintf_(ESPLogLevel level, const std::string &tag, const char *format, va_list args);

 protected:
  uint32_t baud_rate_;
  std::vector<char> tx_buffer_;
  std::string mqtt_logging_topic_;
  bool mqtt_logging_enabled_{true};
  ESPLogLevel global_log_level_{ESPHOMELIB_LOG_LEVEL};
  std::unordered_map<std::string, ESPLogLevel> log_levels_;
};

extern LogComponent *global_log_component;

} // namespace esphomelib

#endif //ESPHOMELIB_LOG_COMPONENT_H
