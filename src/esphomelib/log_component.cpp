//
// Created by Otto Winter on 25.11.17.
//

#include <esp_log.h>
#include <HardwareSerial.h>
#include "log_component.h"
#include "esphomelib/mqtt/mqtt_client_component.h"

namespace esphomelib {

static const char *TAG = "log";

int LogComponent::log_vprintf_(const char *format, va_list args) {
  auto *log = global_log_component;
  int ret = vsnprintf(log->tx_buffer_.data(), log->tx_buffer_.capacity(), format, args);

  if (ret > 0) {
    if (log->baud_rate_ > 0) {
      Serial.print(log->tx_buffer_.data());
    }

    if (!log->mqtt_topic_.empty() && mqtt::global_mqtt_client != nullptr)
      mqtt::global_mqtt_client->publish(log->mqtt_topic_, log->tx_buffer_.data(), true);
  }

  return ret;
}

LogComponent::LogComponent(uint32_t baud_rate, std::string mqtt_topic, size_t tx_buffer_size)
    : baud_rate_(baud_rate), mqtt_topic_(std::move(mqtt_topic)) {
  this->set_tx_buffer_size(tx_buffer_size);
}

void LogComponent::pre_setup() {
  if (this->baud_rate_ > 0)
    Serial.begin(this->baud_rate_);

  global_log_component = this;
  esp_log_set_vprintf(LogComponent::log_vprintf_);

#ifdef LOG_LOCAL_LEVEL
  this->set_global_log_level(LOG_LOCAL_LEVEL);
#endif

  ESP_LOGI(TAG, "Log initialized");
  Serial.println("Log initialized (Serial)");
}
uint32_t LogComponent::get_baud_rate() const {
  return this->baud_rate_;
}
void LogComponent::set_baud_rate(uint32_t baud_rate) {
  this->baud_rate_ = baud_rate;
}
const std::string &LogComponent::get_mqtt_topic() const {
  return this->mqtt_topic_;
}
void LogComponent::set_mqtt_topic(const std::string &mqtt_topic) {
  this->mqtt_topic_ = mqtt_topic;
}
void LogComponent::set_global_log_level(esp_log_level_t log_level) {
  this->set_log_level("*", log_level);
}
void LogComponent::set_log_level(const std::string &tag, esp_log_level_t log_level) {
  esp_log_level_set(tag.c_str(), log_level);
}
size_t LogComponent::get_tx_buffer_size() const {
  return this->tx_buffer_.capacity();
}
void LogComponent::set_tx_buffer_size(size_t tx_buffer_size) {
  this->tx_buffer_.reserve(tx_buffer_size);
}

LogComponent *global_log_component = nullptr;

void __assert_func(const char *file, int lineno, const char *func, const char *exp) {
  ESP_LOGE("assert",
           "assertion \"%s\" failed: file \"%s\", line %d%s%s",
           exp,
           file,
           lineno,
           func ? ", function: " : "",
           func ? func : "");
  abort();
}

} // namespace esphomelib