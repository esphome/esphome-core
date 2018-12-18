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
#include "esphomelib/defines.h"

ESPHOMELIB_NAMESPACE_BEGIN

/** A simple component that enables logging to Serial via the ESP_LOG* macros.
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

  /// Set the buffer size that's used for constructing log messages. Log messages longer than this will be truncated.
  void set_tx_buffer_size(size_t tx_buffer_size);

  /// Set the global log level. Note: Use the ESPHOMELIB_LOG_LEVEL define to also remove the logs from the build.
  void set_global_log_level(int log_level);
  int get_global_log_level() const;

  /// Set the log level of the specified tag.
  void set_log_level(const std::string &tag, int log_level);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Set up this component.
  void pre_setup();
  uint32_t get_baud_rate() const;
  void dump_config() override;

  size_t get_tx_buffer_size() const;

  int log_vprintf_(int level, const char *tag, const char *format, va_list args);

  /// Register a callback that will be called for every log message sent
  void add_on_log_callback(std::function<void(int, const char *, const char *)> &&callback);

  float get_setup_priority() const override;

 protected:
  uint32_t baud_rate_;
  std::vector<char> tx_buffer_;
  int global_log_level_{ESPHOMELIB_LOG_LEVEL};
  struct LogLevelOverride {
    std::string tag;
    int level;
  };
  std::vector<LogLevelOverride> log_levels_;
  CallbackManager<void(int, const char *, const char *)> log_callback_{};
};

extern LogComponent *global_log_component;

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_LOG_COMPONENT_H
