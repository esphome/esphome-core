//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/log_component.h"

#ifdef ARDUINO_ARCH_ESP32
  #include <esp_log.h>
#endif
#include <HardwareSerial.h>

#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/log.h"
#include "esphomelib/application.h"

namespace esphomelib {

static const char *TAG = "log_component";

int LogComponent::log_vprintf_(ESPLogLevel level, const std::string &tag,
                               const char *format, va_list args) {
  auto it = this->log_levels_.find(tag);
  ESPLogLevel max_level = this->global_log_level_;
  //if (it == this->log_levels_.end())
  //  max_level = it->second;

  if (level > max_level)
    return 0;

  int ret = vsnprintf(this->tx_buffer_.data(), this->tx_buffer_.capacity(),
                      format, args);

  if (ret > 0) { // only if format successful
    if (this->baud_rate_ > 0)
      Serial.println(this->tx_buffer_.data());

    if (this->mqtt_logging_enabled_ && mqtt::global_mqtt_client != nullptr &&
        mqtt::global_mqtt_client->is_connected()) // don't try if we're not connected.
      mqtt::global_mqtt_client->publish(
          this->get_logging_topic(), this->tx_buffer_.data(), true);
  }

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
#endif

  ESP_LOGI(TAG, "Log initialized");
}
uint32_t LogComponent::get_baud_rate() const {
  return this->baud_rate_;
}
void LogComponent::set_baud_rate(uint32_t baud_rate) {
  this->baud_rate_ = baud_rate;
}
void LogComponent::set_global_log_level(ESPLogLevel log_level) {
  this->global_log_level_ = log_level;
}
void LogComponent::set_log_level(const std::string &tag, ESPLogLevel log_level) {
  this->log_levels_[tag] = log_level;
}
size_t LogComponent::get_tx_buffer_size() const {
  return this->tx_buffer_.capacity();
}
void LogComponent::set_tx_buffer_size(size_t tx_buffer_size) {
  this->tx_buffer_.reserve(tx_buffer_size);
}
void LogComponent::set_custom_logging_topic(const std::string &custom_logging_topic) {
  this->mqtt_logging_topic_ = custom_logging_topic;
  this->set_mqtt_logging_enabled(true);
}
bool LogComponent::is_mqtt_logging_enabled() const {
  return this->mqtt_logging_enabled_;
}
void LogComponent::set_mqtt_logging_enabled(bool mqtt_logging_enabled) {
  this->mqtt_logging_enabled_ = mqtt_logging_enabled;
}
std::string LogComponent::get_logging_topic() {
  if (this->mqtt_logging_topic_.empty() && mqtt::global_mqtt_client != nullptr)
    return mqtt::global_mqtt_client->get_topic_prefix() + "/debug";
  return this->mqtt_logging_topic_;
}

LogComponent *global_log_component = nullptr;

} // namespace esphomelib