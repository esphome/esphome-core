#include "esphomelib/log_component.h"

#ifdef ARDUINO_ARCH_ESP32
  #include <esp_log.h>
#endif
#include <HardwareSerial.h>

#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "logger";

int HOT LogComponent::log_vprintf_(int level, const char *tag,
                               const char *format, va_list args) {
  // Uses std::vector<> for low memory footprint, though the vector
  // could be sorted to minimize lookup times. This feature isn't used that
  // much anyway so it doesn't matter too much.
  int max_level = this->global_log_level_;
  for (auto &it : this->log_levels_) {
    if (it.tag == tag) {
      max_level = it.level;
      break;
    }
  }

  if (level > max_level)
    return 0;

  int ret = vsnprintf(this->tx_buffer_.data(), this->tx_buffer_.capacity(),
                      format, args);
  if (ret <= 0)
    return ret;

  if (this->baud_rate_ > 0)
    Serial.println(this->tx_buffer_.data());

  this->log_callback_.call(level, this->tx_buffer_.data());
  return ret;
}

LogComponent::LogComponent(uint32_t baud_rate, size_t tx_buffer_size)
    : baud_rate_(baud_rate) {
  this->set_tx_buffer_size(tx_buffer_size);
}

void LogComponent::pre_setup() {
  if (this->baud_rate_ > 0)
    Serial.begin(this->baud_rate_);

  global_log_component = this;
#ifdef ARDUINO_ARCH_ESP32
  esp_log_set_vprintf(esp_idf_log_vprintf_);
  esp_log_level_t log_level;
  switch (this->global_log_level_) {
    case ESPHOMELIB_LOG_LEVEL_ERROR:
      log_level = ESP_LOG_ERROR;
      break;
    case ESPHOMELIB_LOG_LEVEL_WARN:
      log_level = ESP_LOG_WARN;
      break;
    case ESPHOMELIB_LOG_LEVEL_INFO:
      log_level = ESP_LOG_INFO;
      break;
    case ESPHOMELIB_LOG_LEVEL_DEBUG:
      log_level = ESP_LOG_DEBUG;
      break;
    case ESPHOMELIB_LOG_LEVEL_VERBOSE:
    case ESPHOMELIB_LOG_LEVEL_VERY_VERBOSE:
      log_level = ESP_LOG_VERBOSE;
      break;
    case ESPHOMELIB_LOG_LEVEL_NONE:
    default:
      log_level = ESP_LOG_NONE;
      break;
  }
  // esp_log_level_set("*", log_level);
#endif

  ESP_LOGI(TAG, "Log initialized");
}
uint32_t LogComponent::get_baud_rate() const {
  return this->baud_rate_;
}
void LogComponent::set_baud_rate(uint32_t baud_rate) {
  this->baud_rate_ = baud_rate;
}
void LogComponent::set_global_log_level(int log_level) {
  this->global_log_level_ = log_level;
}
void LogComponent::set_log_level(const std::string &tag, int log_level) {
  this->log_levels_.push_back(LogLevelOverride{tag, log_level});
}
size_t LogComponent::get_tx_buffer_size() const {
  return this->tx_buffer_.capacity();
}
void LogComponent::set_tx_buffer_size(size_t tx_buffer_size) {
  this->tx_buffer_.reserve(tx_buffer_size);
}
void LogComponent::add_on_log_callback(std::function<void(int, const char *)> &&callback) {
  this->log_callback_.add(std::move(callback));
}
float LogComponent::get_setup_priority() const {
  return setup_priority::HARDWARE - 1.0f;
}
const char *LOG_LEVELS[] = {"NONE", "ERROR", "WARN", "INFO", "DEBUG", "VERBOSE", "VERY_VERBOSE"};
void LogComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Logger:");
  ESP_LOGCONFIG(TAG, "  Level: %s", LOG_LEVELS[this->global_log_level_]);
  ESP_LOGCONFIG(TAG, "  Log Baud Rate: %u", this->baud_rate_);
  for (auto &it : this->log_levels_) {
    ESP_LOGCONFIG(TAG, "  Level for '%s': %s", it.tag.c_str(), LOG_LEVELS[it.level]);
  }
}
int LogComponent::get_global_log_level() const {
  return this->global_log_level_;
}

LogComponent *global_log_component = nullptr;

ESPHOMELIB_NAMESPACE_END
