//
// Created by Otto Winter on 25.11.17.
//

#include <cstdio>
#include <algorithm>

#ifdef ARDUINO_ARCH_ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <Esp.h>
#endif

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

static const char *TAG = "helpers";

std::string get_mac_address() {
  char tmp[20];
  uint8_t mac[6];
#ifdef ARDUINO_ARCH_ESP32
  esp_efuse_mac_get_default(mac);
#endif
#ifdef ARDUINO_ARCH_ESP8266
  WiFi.macAddress(mac);
#endif
  sprintf(tmp, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return std::string(tmp);
}

bool is_empty(const IPAddress &address) {
  return address == IPAddress(0, 0, 0, 0);
}

std::string generate_hostname(const std::string &base) {
  return base + std::string("-") + get_mac_address();
}

uint32_t random_uint32() {
#ifdef ARDUINO_ARCH_ESP32
  return esp_random();
#else
  return os_random();
#endif
}

double random_double() {
  return random_uint32() / double(UINT32_MAX);
}

float random_float() {
  return float(random_double());
}

float gamma_correct(float value, float gamma) {
  if (value <= 0.0f)
    return 0.0f;
  if (gamma <= 0.0f)
    return value;

  return powf(value, gamma);
}
std::string to_lowercase_underscore(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  std::replace(s.begin(), s.end(), ' ', '_');
  return s;
}

std::string sanitize_string_whitelist(const std::string &s, const std::string &whitelist) {
  std::string out(s);
  out.erase(std::remove_if(out.begin(), out.end(), [&out, &whitelist](const char &c) {
    return whitelist.find(c) == std::string::npos;
  }), out.end());
  return out;
}

std::string sanitize_hostname(const std::string &hostname) {
  std::string s = sanitize_string_whitelist(hostname, HOSTNAME_CHARACTER_WHITELIST);
  return truncate_string(s, 63);
}

std::string truncate_string(const std::string &s, size_t length) {
  if (s.length() > length)
    return s.substr(0, length);
  return s;
}

ExponentialMovingAverage::ExponentialMovingAverage(float alpha) : alpha_(alpha), accumulator_(0) {}

float ExponentialMovingAverage::get_alpha() const {
  return this->alpha_;
}

void ExponentialMovingAverage::set_alpha(float alpha) {
  this->alpha_ = alpha;
}

float ExponentialMovingAverage::calculate_average() {
  return this->accumulator_;
}

float ExponentialMovingAverage::next_value(float value) {
  this->accumulator_ = (this->alpha_ * value) + (1.0f - this->alpha_) * this->accumulator_;
  return this->calculate_average();
}

void run_without_interrupts(const std::function<void()> &f) {
#ifdef ARDUINO_ARCH_ESP32
  portDISABLE_INTERRUPTS();
#else
  noInterrupts();
#endif
  f();
#ifdef ARDUINO_ARCH_ESP32
  portENABLE_INTERRUPTS();
#else
  interrupts();
#endif
}
std::string value_accuracy_to_string(float value, int8_t accuracy_decimals) {
  auto multiplier = float(pow10(accuracy_decimals));
  float value_rounded = roundf(value * multiplier) / multiplier;
  char tmp[32]; // should be enough, but we should maybe improve this at some point.
  dtostrf(value_rounded, 0, uint8_t(std::max(0, int(accuracy_decimals))), tmp);
  return std::string(tmp);
}
std::string uint64_to_string(uint64_t num) {
  char buffer[17];
  auto *address16 = reinterpret_cast<uint16_t *>(&num);
  snprintf(buffer, sizeof(buffer), "%04X%04X%04X%04X",
           address16[3], address16[2], address16[1], address16[0]);
  return std::string(buffer);
}
std::string build_json(const json_build_t &f) {
  StaticJsonBuffer<JSON_BUFFER_SIZE> json_buffer;
  JsonObject &root = json_buffer.createObject();

  f(json_buffer, root);

  std::string buffer;
  root.printTo(buffer);
  return buffer;
}
void parse_json(const std::string &data, const json_parse_t &f) {
  StaticJsonBuffer<JSON_BUFFER_SIZE> buffer;
  JsonObject &root = buffer.parseObject(data);

  if (!root.success()) {
    ESP_LOGW(TAG, "Parsing JSON failed.");
    return;
  }

  f(root);
}
Optional<bool> parse_on_off(const char *str, const char *payload_on, const char *payload_off) {
  if (strcasecmp(str, payload_on) == 0)
    return true;
  if (strcasecmp(str, payload_off) == 0)
    return false;

  return Optional<bool>();
}

CallbackManager<void()> shutdown_hooks;
CallbackManager<void()> safe_shutdown_hooks;

void reboot() {
  ESP_LOGI(TAG, "Forcing a reboot...");
  run_shutdown_hooks();
  ESP.restart();
}
void add_shutdown_hook(std::function<void()> &&f) {
  shutdown_hooks.add(std::move(f));
}
void safe_reboot() {
  ESP_LOGI(TAG, "Rebooting safely...");
  run_safe_shutdown_hooks();
  ESP.restart();
}
void add_safe_shutdown_hook(std::function<void()> &&f) {
  safe_shutdown_hooks.add(std::move(f));
}

void run_shutdown_hooks() {
  shutdown_hooks.call();
}

void run_safe_shutdown_hooks() {
  safe_shutdown_hooks.call();
  shutdown_hooks.call();
}

const char *HOSTNAME_CHARACTER_WHITELIST = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";

} // namespace esphomelib
