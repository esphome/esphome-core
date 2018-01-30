//
// Created by Otto Winter on 25.11.17.
//

#include "helpers.h"
#include <cstdio>
#include <Esp.h>
#include <algorithm>
#include <esphomelib/espmath.h>

namespace esphomelib {

static const char *TAG = "helpers";

std::string get_mac_address() {
  char tmp[20];
  uint8_t mac[6];
  esp_efuse_mac_get_default(mac);
  sprintf(tmp, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return std::string(tmp);
}

bool is_empty(const IPAddress &address) {
  return address == IPAddress(0, 0, 0, 0);
}

std::string generate_hostname(const std::string &base) {
  return base + std::string("-") + get_mac_address();
}

double random_double() {
  return double(esp_random()) / double(UINT32_MAX);
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

template<>
void run_without_interrupts<void>(const std::function<void()> &f) {
  portDISABLE_INTERRUPTS();
  f();
  portENABLE_INTERRUPTS();
}

} // namespace esphomelib
