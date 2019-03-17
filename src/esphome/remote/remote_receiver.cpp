#include "esphome/defines.h"

#ifdef USE_REMOTE_RECEIVER

#include "esphome/remote/remote_receiver.h"
#include "esphome/log.h"
#include "esphome/remote/jvc.h"
#include "esphome/remote/nec.h"
#include "esphome/remote/lg.h"
#include "esphome/remote/panasonic.h"
#include "esphome/remote/remote_transmitter.h"
#include "esphome/remote/rc_switch.h"
#include "esphome/remote/samsung.h"
#include "esphome/remote/sony.h"

#ifdef ARDUINO_ARCH_ESP32
#include <soc/rmt_struct.h>
#endif

ESPHOME_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.receiver";

RemoteReceiveData::RemoteReceiveData(RemoteReceiverComponent *parent, std::vector<int32_t> *data)
    : parent_(parent), data_(data) {}

uint32_t RemoteReceiveData::lower_bound_(uint32_t length) {
  return uint32_t(100 - this->parent_->tolerance_) * length / 100U;
}
uint32_t RemoteReceiveData::upper_bound_(uint32_t length) {
  return uint32_t(100 + this->parent_->tolerance_) * length / 100U;
}
bool RemoteReceiveData::peek_mark(uint32_t length, uint32_t offset) {
  if (int32_t(this->index_ + offset) >= this->size())
    return false;
  int32_t value = this->peek(offset);
  const int32_t lo = this->lower_bound_(length);
  const int32_t hi = this->upper_bound_(length);
  return value >= 0 && lo <= value && value <= hi;
}
bool RemoteReceiveData::peek_space(uint32_t length, uint32_t offset) {
  if (int32_t(this->index_ + offset) >= this->size())
    return false;
  int32_t value = this->peek(offset);
  const int32_t lo = this->lower_bound_(length);
  const int32_t hi = this->upper_bound_(length);
  return value <= 0 && lo <= -value && -value <= hi;
}
bool RemoteReceiveData::peek_item(uint32_t mark, uint32_t space, uint32_t offset) {
  return this->peek_mark(mark, offset) && this->peek_space(space, offset + 1);
}
void RemoteReceiveData::advance(uint32_t amount) { this->index_ += amount; }
bool RemoteReceiveData::expect_mark(uint32_t length) {
  if (this->peek_mark(length)) {
    this->advance();
    return true;
  }
  return false;
}
bool RemoteReceiveData::expect_space(uint32_t length) {
  if (this->peek_space(length)) {
    this->advance();
    return true;
  }
  return false;
}
bool RemoteReceiveData::expect_item(uint32_t mark, uint32_t space) {
  if (this->peek_item(mark, space)) {
    this->advance(2);
    return true;
  }
  return false;
}
void RemoteReceiveData::reset_index() { this->index_ = 0; }
int32_t RemoteReceiveData::peek(uint32_t offset) { return (*this)[this->index_ + offset]; }
bool RemoteReceiveData::peek_space_at_least(uint32_t length, uint32_t offset) {
  if (int32_t(this->index_ + offset) >= this->size())
    return false;
  int32_t value = this->pos(this->index_ + offset);
  const int32_t lo = this->lower_bound_(length);
  return value <= 0 && lo <= -value;
}
int32_t RemoteReceiveData::operator[](uint32_t index) const { return this->pos(index); }
int32_t RemoteReceiveData::pos(uint32_t index) const { return (*this->data_)[index]; }

int32_t RemoteReceiveData::size() const { return this->data_->size(); }
JVCDecodeData RemoteReceiveData::decode_jvc() { return remote::decode_jvc(this); }
LGDecodeData RemoteReceiveData::decode_lg() { return remote::decode_lg(this); }
NECDecodeData RemoteReceiveData::decode_nec() { return remote::decode_nec(this); }
PanasonicDecodeData RemoteReceiveData::decode_panasonic() { return remote::decode_panasonic(this); }
SamsungDecodeData RemoteReceiveData::decode_samsung() { return remote::decode_samsung(this); }
SonyDecodeData RemoteReceiveData::decode_sony() { return remote::decode_sony(this); }

RemoteReceiverComponent::RemoteReceiverComponent(GPIOPin *pin) : RemoteControlComponentBase(pin) {}

float RemoteReceiverComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

#ifdef ARDUINO_ARCH_ESP32
void RemoteReceiverComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Remote Receiver...");
  rmt_config_t rmt{};
  rmt.channel = this->channel_;
  rmt.gpio_num = gpio_num_t(this->pin_->get_pin());
  rmt.clk_div = this->clock_divider_;
  rmt.mem_block_num = 1;
  rmt.rmt_mode = RMT_MODE_RX;
  if (this->filter_us_ == 0) {
    rmt.rx_config.filter_en = false;
  } else {
    rmt.rx_config.filter_en = true;
    rmt.rx_config.filter_ticks_thresh = this->from_microseconds(this->filter_us_);
  }
  rmt.rx_config.idle_threshold = this->from_microseconds(this->idle_us_);

  esp_err_t error = rmt_config(&rmt);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->mark_failed();
    return;
  }

  error = rmt_driver_install(this->channel_, this->buffer_size_, 0);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->mark_failed();
    return;
  }
  error = rmt_get_ringbuf_handle(this->channel_, &this->ringbuf_);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->mark_failed();
    return;
  }
  error = rmt_rx_start(this->channel_, true);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->mark_failed();
    return;
  }
}
void RemoteReceiverComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Remote Receiver:");
  LOG_PIN("  Pin: ", this->pin_);
  ESP_LOGCONFIG(TAG, "  Channel: %d", this->channel_);
  ESP_LOGCONFIG(TAG, "  Clock divider: %u", this->clock_divider_);
  ESP_LOGCONFIG(TAG, "  Tolerance: %u%%", this->tolerance_);
  ESP_LOGCONFIG(TAG, "  Filter out pulses shorter than: %u us", this->filter_us_);
  ESP_LOGCONFIG(TAG, "  Signal is done after %u us of no changes", this->idle_us_);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Configuring RMT driver failed: %s", esp_err_to_name(this->error_code_));
  }

  for (auto *child : this->decoders_) {
    LOG_BINARY_SENSOR("  ", "Binary Sensor", child);
  }
}

void RemoteReceiverComponent::loop() {
  size_t len = 0;
  auto *item = (rmt_item32_t *) xRingbufferReceive(this->ringbuf_, &len, 0);
  if (item != nullptr) {
    this->decode_rmt_(item, len);
    vRingbufferReturnItem(this->ringbuf_, item);

    if (this->temp_.empty())
      return;

    RemoteReceiveData data(this, &this->temp_);
    this->process_(&data);
  }
}
void RemoteReceiverComponent::decode_rmt_(rmt_item32_t *item, size_t len) {
  bool prev_level = false;
  uint32_t prev_length = 0;
  this->temp_.clear();
  int32_t multiplier = this->pin_->is_inverted() ? -1 : 1;

  ESP_LOGVV(TAG, "START:");
  for (size_t i = 0; i < len; i++) {
    if (item[i].level0) {
      ESP_LOGVV(TAG, "%u A: ON %uus (%u ticks)", i, this->to_microseconds(item[i].duration0), item[i].duration0);
    } else {
      ESP_LOGVV(TAG, "%u A: OFF %uus (%u ticks)", i, this->to_microseconds(item[i].duration0), item[i].duration0);
    }
    if (item[i].level1) {
      ESP_LOGVV(TAG, "%u B: ON %uus (%u ticks)", i, this->to_microseconds(item[i].duration1), item[i].duration1);
    } else {
      ESP_LOGVV(TAG, "%u B: OFF %uus (%u ticks)", i, this->to_microseconds(item[i].duration1), item[i].duration1);
    }
  }
  ESP_LOGVV(TAG, "\n");

  this->temp_.reserve(len / 4);
  for (size_t i = 0; i < len; i++) {
    if (item[i].duration0 == 0u) {
      // Do nothing
    } else if (bool(item[i].level0) == prev_level) {
      prev_length += item[i].duration0;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          this->temp_.push_back(this->to_microseconds(prev_length) * multiplier);
        } else {
          this->temp_.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
        }
      }
      prev_level = bool(item[i].level0);
      prev_length = item[i].duration0;
    }

    if (this->to_microseconds(prev_length) > this->idle_us_) {
      break;
    }

    if (item[i].duration1 == 0u) {
      // Do nothing
    } else if (bool(item[i].level1) == prev_level) {
      prev_length += item[i].duration1;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          this->temp_.push_back(this->to_microseconds(prev_length) * multiplier);
        } else {
          this->temp_.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
        }
      }
      prev_level = bool(item[i].level1);
      prev_length = item[i].duration1;
    }

    if (this->to_microseconds(prev_length) > this->idle_us_) {
      break;
    }
  }
  if (prev_length > 0) {
    if (prev_level) {
      this->temp_.push_back(this->to_microseconds(prev_length) * multiplier);
    } else {
      this->temp_.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
    }
  }
}
#endif

#ifdef ARDUINO_ARCH_ESP8266

void ICACHE_RAM_ATTR HOT RemoteReceiverComponentStore::gpio_intr(RemoteReceiverComponentStore *arg) {
  const uint32_t now = micros();
  // If the lhs is 1 (rising edge) we should write to an uneven index and vice versa
  const uint32_t next = (arg->buffer_write_at + 1) % arg->buffer_size;
  if (uint32_t(arg->pin->digital_read()) != next % 2)
    return;
  const uint32_t last_change = arg->buffer[arg->buffer_write_at];
  if (now - last_change <= arg->filter_us)
    return;

  arg->buffer[arg->buffer_write_at = next] = now;

  if (next == arg->buffer_read_at) {
    arg->overflow = true;
  }
}

void RemoteReceiverComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Remote Receiver...");
  this->pin_->setup();
  auto &s = this->store_;
  s.filter_us = this->filter_us_;
  s.pin = this->pin_->to_isr();
  s.buffer_size = this->buffer_size_;

  this->high_freq_.start();
  if (s.buffer_size % 2 != 0) {
    // Make sure divisible by two. This way, we know that every 0bxxx0 index is a space and every 0bxxx1 index is a mark
    s.buffer_size++;
  }
  s.buffer = new uint32_t[s.buffer_size];
  // First index is a space.
  if (this->pin_->digital_read()) {
    s.buffer_write_at = s.buffer_read_at = 1;
    s.buffer[1] = 0;
    s.buffer[0] = 0;
  } else {
    s.buffer_write_at = s.buffer_read_at = 0;
    s.buffer[0] = 0;
  }
  this->pin_->attach_interrupt(RemoteReceiverComponentStore::gpio_intr, &this->store_, CHANGE);
}
void RemoteReceiverComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Remote Receiver:");
  LOG_PIN("  Pin: ", this->pin_);
  if (this->pin_->digital_read()) {
    ESP_LOGW(TAG, "Remote Receiver Signal starts with a HIGH value. Usually this means you have to "
                  "invert the signal using 'inverted: True' in the pin schema!");
  }
  ESP_LOGCONFIG(TAG, "  Buffer Size: %u", this->buffer_size_);
  ESP_LOGCONFIG(TAG, "  Tolerance: %u%%", this->tolerance_);
  ESP_LOGCONFIG(TAG, "  Filter out pulses shorter than: %u us", this->filter_us_);
  ESP_LOGCONFIG(TAG, "  Signal is done after %u us of no changes", this->idle_us_);

  for (auto *child : this->decoders_) {
    LOG_BINARY_SENSOR("  ", "Binary Sensor", child);
  }
}

void RemoteReceiverComponent::loop() {
  auto &s = this->store_;
  if (s.overflow) {
    s.buffer_read_at = s.buffer_write_at;
    s.overflow = false;
    ESP_LOGW(TAG, "Data is coming in too fast! Try increasing the buffer size.");
    return;
  }

  // copy write at to local variables, as it's volatile
  const uint32_t write_at = s.buffer_write_at;
  const uint32_t dist = (s.buffer_size + write_at - s.buffer_read_at) % s.buffer_size;
  // signals must at least one rising and one leading edge
  if (dist <= 1)
    return;
  const uint32_t now = micros();
  if (now - s.buffer[write_at] < this->idle_us_)
    // The last change was fewer than the configured idle time ago.
    // TODO: Handle case when loop() is not called quickly enough to catch idle
    return;

  ESP_LOGVV(TAG, "read_at=%u write_at=%u dist=%u now=%u end=%u", s.buffer_read_at, write_at, dist, now,
            s.buffer[write_at]);

  // Skip first value, it's from the previous idle level
  s.buffer_read_at = (s.buffer_read_at + 1) % s.buffer_size;
  uint32_t prev = s.buffer_read_at;
  s.buffer_read_at = (s.buffer_read_at + 1) % s.buffer_size;
  const uint32_t reserve_size = 1 + (s.buffer_size + write_at - s.buffer_read_at) % s.buffer_size;
  this->temp_.clear();
  this->temp_.reserve(reserve_size);
  int32_t multiplier = s.buffer_read_at % 2 == 0 ? 1 : -1;

  for (uint32_t i = 0; prev != write_at; i++) {
    int32_t delta = s.buffer[s.buffer_read_at] - s.buffer[prev];
    if (uint32_t(delta) >= this->idle_us_) {
      // already found a space longer than idle. There must have been two pulses
      break;
    }

    ESP_LOGVV(TAG, "  i=%u buffer[%u]=%u - buffer[%u]=%u -> %d", i, s.buffer_read_at, s.buffer[s.buffer_read_at], prev,
              s.buffer[prev], multiplier * delta);
    this->temp_.push_back(multiplier * delta);
    prev = s.buffer_read_at;
    s.buffer_read_at = (s.buffer_read_at + 1) % s.buffer_size;
    multiplier *= -1;
  }
  s.buffer_read_at = (s.buffer_size + s.buffer_read_at - 1) % s.buffer_size;
  this->temp_.push_back(this->idle_us_ * multiplier);

  RemoteReceiveData data(this, &this->temp_);
  this->process_(&data);
}
#endif

RemoteReceiver *RemoteReceiverComponent::add_decoder(RemoteReceiver *decoder) {
  this->decoders_.push_back(decoder);
  return decoder;
}
void RemoteReceiverComponent::add_dumper(RemoteReceiveDumper *dumper) { this->dumpers_.push_back(dumper); }
void RemoteReceiverComponent::set_buffer_size(uint32_t buffer_size) { this->buffer_size_ = buffer_size; }
void RemoteReceiverComponent::set_tolerance(uint8_t tolerance) { this->tolerance_ = tolerance; }
void RemoteReceiverComponent::set_filter_us(uint8_t filter_us) { this->filter_us_ = filter_us; }
void RemoteReceiverComponent::set_idle_us(uint32_t idle_us) { this->idle_us_ = idle_us; }
void RemoteReceiverComponent::process_(RemoteReceiveData *data) {
  bool found_decoder = false;
  for (auto *decoder : this->decoders_) {
    if (decoder->process(data))
      found_decoder = true;
  }

  if (!found_decoder) {
    bool found = false;

    for (auto *dumper : this->dumpers_) {
      if (!dumper->is_secondary()) {
        if (dumper->process(data)) {
          found = true;
        }
      }
    }

    for (auto *dumper : this->dumpers_) {
      if (!found && dumper->is_secondary()) {
        dumper->process(data);
      }
    }
  }
}

RemoteReceiver::RemoteReceiver(const std::string &name) : BinarySensor(name) {}

bool RemoteReceiver::process(RemoteReceiveData *data) {
  data->reset_index();
  if (this->matches(data)) {
    this->publish_state(true);
    yield();
    this->publish_state(false);
    return true;
  }
  return false;
}
bool RemoteReceiveDumper::is_secondary() { return false; }

bool RemoteReceiveDumper::process(RemoteReceiveData *data) {
  data->reset_index();
  return this->dump(data);
}

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE_RECEIVER
