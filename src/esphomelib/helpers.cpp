#include <cstdio>
#include <algorithm>

#ifdef ARDUINO_ARCH_ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <Esp.h>
#endif

#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"
#include "esphomelib/log.h"
#include "esphomelib/esphal.h"
#include "esphomelib/espmath.h"
#include "esphomelib/status_led.h"

ESPHOMELIB_NAMESPACE_BEGIN

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
  if (std::isnan(value)) {
    return this->calculate_average();
  }

  if (this->first_value_) {
    this->accumulator_ = value;
  } else {
    this->accumulator_ = (this->alpha_ * value) + (1.0f - this->alpha_) * this->accumulator_;
  }
  this->first_value_ = false;
  return this->calculate_average();
}

SlidingWindowMovingAverage::SlidingWindowMovingAverage(size_t max_size) : max_size_(max_size), sum_(0) {

}

float SlidingWindowMovingAverage::next_value(float value) {
  if (std::isnan(value))
    return this->calculate_average();
  if (this->queue_.size() == this->max_size_) {
    this->sum_ -= this->queue_.front();
    this->queue_.pop();
  }
  this->queue_.push(value);
  this->sum_ += value;

  return this->calculate_average();
}

float SlidingWindowMovingAverage::calculate_average() {
  if (this->queue_.size() == 0)
    return 0;
  else
    return this->sum_ / this->queue_.size();
}

size_t SlidingWindowMovingAverage::get_max_size() const {
  return this->max_size_;
}

void SlidingWindowMovingAverage::set_max_size(size_t max_size) {
  this->max_size_ = max_size;

  while (this->queue_.size() > max_size) {
    this->sum_ -= this->queue_.front();
    this->queue_.pop();
  }
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
std::string uint32_to_string(uint32_t num) {
  char buffer[9];
  auto *address16 = reinterpret_cast<uint16_t *>(&num);
  snprintf(buffer, sizeof(buffer), "%04X%04X", address16[1], address16[0]);
  return std::string(buffer);
}
static char *global_json_build_buffer = nullptr;
static size_t global_json_build_buffer_size = 0;

void reserve_global_json_build_buffer(size_t required_size) {
  if (global_json_build_buffer_size == 0 || global_json_build_buffer_size < required_size) {
    delete [] global_json_build_buffer;
    global_json_build_buffer_size = std::max(required_size, global_json_build_buffer_size * 2);

    size_t remainder = global_json_build_buffer_size % 16U;
    if (remainder != 0)
      global_json_build_buffer_size += 16 - remainder;

    global_json_build_buffer = new char[global_json_build_buffer_size];
  }
}

const char *build_json(const json_build_t &f, size_t *length) {
  global_json_buffer.clear();
  JsonObject &root = global_json_buffer.createObject();

  f(root);

  // The Json buffer size gives us a good estimate for the required size.
  // Usually, it's a bit larger than the actual required string size
  //             | JSON Buffer Size | String Size |
  // Discovery   | 388              | 351         |
  // Discovery   | 372              | 356         |
  // Discovery   | 336              | 311         |
  // Discovery   | 408              | 393         |
  reserve_global_json_build_buffer(global_json_buffer.size());
  size_t bytes_written = root.printTo(global_json_build_buffer, global_json_build_buffer_size);

  if (bytes_written == global_json_build_buffer_size) {
    reserve_global_json_build_buffer(root.measureLength() + 1);
    bytes_written = root.printTo(global_json_build_buffer, global_json_build_buffer_size);
  }

  *length = bytes_written;
  return global_json_build_buffer;
}
void parse_json(const std::string &data, const json_parse_t &f) {
  global_json_buffer.clear();
  JsonObject &root = global_json_buffer.parseObject(data);

  if (!root.success()) {
    ESP_LOGW(TAG, "Parsing JSON failed.");
    return;
  }

  f(root);
}
ParseOnOffState parse_on_off(const char *str, const char *on, const char *off) {
  if (on == nullptr && strcasecmp(str, "on") == 0)
    return PARSE_ON;
  if (on != nullptr && strcasecmp(str, on) == 0)
    return PARSE_ON;
  if (off == nullptr && strcasecmp(str, "off") == 0)
    return PARSE_OFF;
  if (off != nullptr && strcasecmp(str, off) == 0)
    return PARSE_OFF;
  if (strcasecmp(str, "toggle") == 0)
    return PARSE_TOGGLE;

  return PARSE_NONE;
}

CallbackManager<void(const char *)> shutdown_hooks;
CallbackManager<void(const char *)> safe_shutdown_hooks;

void reboot(const char *cause) {
  ESP_LOGI(TAG, "Forcing a reboot... Reason: '%s'", cause);
  run_shutdown_hooks(cause);
  ESP.restart();
  // restart() doesn't always end execution
  while (true) {
    yield();
  }
}
void add_shutdown_hook(std::function<void(const char *)> &&f) {
  shutdown_hooks.add(std::move(f));
}
void safe_reboot(const char *cause) {
  ESP_LOGI(TAG, "Rebooting safely... Reason: '%s'", cause);
  run_safe_shutdown_hooks(cause);
  ESP.restart();
  // restart() doesn't always end execution
  while (true) {
    yield();
  }
}
void add_safe_shutdown_hook(std::function<void(const char *)> &&f) {
  safe_shutdown_hooks.add(std::move(f));
}

void run_shutdown_hooks(const char *cause) {
  shutdown_hooks.call(cause);
}

void run_safe_shutdown_hooks(const char *cause) {
  safe_shutdown_hooks.call(cause);
  shutdown_hooks.call(cause);
}

const char *HOSTNAME_CHARACTER_WHITELIST = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";

void disable_interrupts() {
#ifdef ARDUINO_ARCH_ESP32
  portDISABLE_INTERRUPTS();
#else
  noInterrupts();
#endif
}
void enable_interrupts() {
#ifdef ARDUINO_ARCH_ESP32
  portENABLE_INTERRUPTS();
#else
  interrupts();
#endif
}

uint8_t crc8(uint8_t *data, uint8_t len) {
  uint8_t crc = 0;

  while ((len--) != 0u) {
    uint8_t inbyte = *data++;
    for (uint8_t i = 8; i != 0u; i--) {
      bool mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix)
        crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}
void delay_microseconds_accurate(uint32_t usec) {
  if (usec == 0)
    return;

  if (usec <= 16383UL) {
    delayMicroseconds(usec);
  } else {
    delay(usec / 1000UL);
    delayMicroseconds(usec % 1000UL);
  }
}

#ifdef ARDUINO_ARCH_ESP32
rmt_channel_t next_rmt_channel = RMT_CHANNEL_0;

rmt_channel_t select_next_rmt_channel() {
  rmt_channel_t value = next_rmt_channel;
  next_rmt_channel = rmt_channel_t(int(next_rmt_channel) + 1); // NOLINT
  return value;
}
#endif

uint8_t reverse_bits_8(uint8_t x) {
  x = ((x & 0xAA) >> 1) | ((x & 0x55) << 1);
  x = ((x & 0xCC) >> 2) | ((x & 0x33) << 2);
  x = ((x & 0xF0) >> 4) | ((x & 0x0F) << 4);
  return x;
}

uint16_t reverse_bits_16(uint16_t x) {
  return uint16_t(reverse_bits_8(x & 0xFF) << 8) | uint16_t(reverse_bits_8(x >> 8));
}
void tick_status_led() {
#ifdef USE_STATUS_LED
  if (global_status_led != nullptr) {
    global_status_led->loop_();
  }
#endif
}
void ICACHE_RAM_ATTR HOT feed_wdt() {
  static uint32_t last_feed = 0;
  uint32_t now = millis();
  if (now - last_feed > 3) {
#ifdef ARDUINO_ARCH_ESP8266
    ESP.wdtFeed();
#endif
#ifdef ARDUINO_ARCH_ESP32
    yield();
#endif
  }
  last_feed = now;
}
std::string build_json(const json_build_t &f) {
  size_t len;
  const char *c_str = build_json(f, &len);
  return std::string(c_str, len);
}

template<uint32_t>
uint32_t reverse_bits(uint32_t x) {
  return uint32_t(reverse_bits_16(x & 0xFFFF) << 16) | uint32_t(reverse_bits_16(x >> 16));
}

VectorJsonBuffer::String::String(VectorJsonBuffer *parent)
    : parent_(parent), start_(parent->size_) {

}
void VectorJsonBuffer::String::append(char c) const {
  char* last = static_cast<char*>(this->parent_->do_alloc(1));
  *last = c;
}
const char *VectorJsonBuffer::String::c_str() const {
  this->append('\0');
  return &this->parent_->buffer_[this->start_];
}
void VectorJsonBuffer::clear() {
  for (char *block : this->free_blocks_)
    free(block);

  this->size_ = 0;
  this->free_blocks_.clear();
}
VectorJsonBuffer::String VectorJsonBuffer::startString() {
  return {this};
}
void *VectorJsonBuffer::alloc(size_t bytes) {
  // Make sure memory addresses are aligned
  uint32_t new_size = round_size_up(this->size_);
  this->resize(new_size);
  return this->do_alloc(bytes);
}
void *VectorJsonBuffer::do_alloc(size_t bytes) {
  const uint32_t begin = this->size_;
  this->resize(begin + bytes);
  return &this->buffer_[begin];
}
void VectorJsonBuffer::resize(size_t size) {
  if (size <= this->size_) {
    this->size_ = size;
    return;
  }

  this->reserve(size);
  this->size_ = size;
}
void VectorJsonBuffer::reserve(size_t size) {
  if (size <= this->capacity_)
    return;

  uint32_t target_capacity = this->capacity_;
  if (this->capacity_ == 0) {
    // lazily initialize with a reasonable size
    target_capacity = JSON_BUFFER_SIZE;
  }
  while (target_capacity < size)
    target_capacity *= 2;

  char *old_buffer = this->buffer_;
  this->buffer_ = new char[target_capacity];
  if (old_buffer != nullptr && this->capacity_ != 0) {
    this->free_blocks_.push_back(old_buffer);
    memcpy(this->buffer_, old_buffer, this->capacity_);
  }
  this->capacity_ = target_capacity;
}

size_t VectorJsonBuffer::size() const {
  return this->size_;
}

VectorJsonBuffer global_json_buffer;

static int high_freq_num_requests = 0;

void HighFrequencyLoopRequester::start() {
  if (this->started_)
    return;
  high_freq_num_requests++;
  this->started_ = true;
}
void HighFrequencyLoopRequester::stop() {
  if (!this->started_)
    return;
  high_freq_num_requests--;
  this->started_ = false;
}
bool HighFrequencyLoopRequester::is_high_frequency() {
  return high_freq_num_requests > 0;
}

ESPHOMELIB_NAMESPACE_END
