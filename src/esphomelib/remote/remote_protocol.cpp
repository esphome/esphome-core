//
//  remote_protocol.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/remote/remote_protocol.h"
#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"

#ifdef USE_REMOTE

#ifdef ARDUINO_ARCH_ESP32
  #include <driver/rmt.h>
  #include <soc/rmt_struct.h>
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.base";

RemoteReceiveData::RemoteReceiveData(RemoteReceiverComponent *parent, const std::vector<int32_t> &data)
    : parent_(parent), data_(data) {}
uint32_t RemoteReceiveData::lower_bound_(uint32_t length) {
  return uint32_t(100 - this->parent_->tolerance_) * length / 100U;
}
uint32_t RemoteReceiveData::upper_bound_(uint32_t length) {
  return uint32_t(100 + this->parent_->tolerance_) * length / 100U;
}
const std::vector<int32_t> &RemoteReceiveData::get_data() const {
  return this->data_;
}
bool RemoteReceiveData::peek_mark(uint32_t length, uint32_t offset) {
  if (this->index_ + offset >= this->size())
    return false;
  int32_t value = data_[this->index_ + offset];
  const int32_t lo = this->lower_bound_(length);
  const int32_t hi = this->upper_bound_(length);
  return value >= 0 && lo <= value && value <= hi;
}
bool RemoteReceiveData::peek_space(uint32_t length, uint32_t offset) {
  if (this->index_ + offset >= this->size())
    return false;
  int32_t value = data_[this->index_ + offset];
  const int32_t lo = this->lower_bound_(length);
  const int32_t hi = this->upper_bound_(length);
  return value <= 0 && lo <= -value && -value <= hi;
}
bool RemoteReceiveData::peek_item(uint32_t mark, uint32_t space, uint32_t offset) {
  return this->peek_mark(mark, offset) && this->peek_space(space, offset + 1);
}
void RemoteReceiveData::advance(uint32_t amount) {
  this->index_ += amount;
}
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
void RemoteReceiveData::reset_index() {
  this->index_ = 0;
}
int32_t RemoteReceiveData::peek(uint32_t offset) {
  return this->data_[this->index_ + offset];
}
bool RemoteReceiveData::peek_space_at_least(uint32_t length, uint32_t offset) {
  if (this->index_ + offset >= this->size())
    return false;
  int32_t value = data_[this->index_ + offset];
  const int32_t lo = this->lower_bound_(length);
  return value <= 0 && lo <= -value;
}
void RemoteTransmitData::mark(uint32_t length) {
  this->data_.push_back(length);
}
void RemoteTransmitData::space(uint32_t length) {
  this->data_.push_back(-length);
}
void RemoteTransmitData::item(uint32_t mark, uint32_t space) {
  this->mark(mark);
  this->space(space);
}
void RemoteTransmitData::reserve(uint32_t len) {
  this->data_.reserve(len);
}
void RemoteTransmitData::set_data(std::vector<int32_t> data) {
  this->data_ = std::move(data);
}
void RemoteTransmitData::set_carrier_frequency(uint32_t carrier_frequency) {
  this->carrier_frequency_ = carrier_frequency;
}
uint32_t RemoteTransmitData::get_carrier_frequency() const {
  return this->carrier_frequency_;
}
const std::vector<int32_t> &RemoteTransmitData::get_data() const {
  return this->data_;
}
RemoteControlComponentBase::RemoteControlComponentBase(GPIOPin *pin)
    : pin_(pin) {
#ifdef ARDUINO_ARCH_ESP32
  this->channel_ = select_next_rmt_channel();
#endif
}
#ifdef ARDUINO_ARCH_ESP32
uint32_t RemoteControlComponentBase::from_microseconds(uint32_t us) {
  const uint32_t ticks_per_ten_us = 80000000u / this->clock_divider_ / 100000u;
  return us * ticks_per_ten_us / 10;
}
uint32_t RemoteControlComponentBase::to_microseconds(uint32_t ticks) {
  const uint32_t ticks_per_ten_us = 80000000u / this->clock_divider_ / 100000u;
  return (ticks * 10) / ticks_per_ten_us;
}
void RemoteControlComponentBase::set_channel(rmt_channel_t channel) {
  this->channel_ = channel;
}
void RemoteControlComponentBase::set_clock_divider(uint8_t clock_divider) {
  this->clock_divider_ = clock_divider;
}
#endif

RemoteReceiverComponent::RemoteReceiverComponent(GPIOPin *pin)
    : RemoteControlComponentBase(pin) {

}

float RemoteReceiverComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

#ifdef ARDUINO_ARCH_ESP32
void RemoteReceiverComponent::setup() {
  this->configure_rmt_();
  rmt_driver_install(this->channel_, this->buffer_size_, 0);
  rmt_get_ringbuf_handle(this->channel_, &this->ringbuf_);
  rmt_rx_start(this->channel_, true);
}


void RemoteReceiverComponent::configure_rmt_() {
  rmt_config_t rmt{};
  ESP_LOGCONFIG(TAG, "Configuring ESP32 RMT peripheral...");
  ESP_LOGCONFIG(TAG, "    Channel: %u", this->channel_);
  rmt.channel = this->channel_;
  ESP_LOGCONFIG(TAG, "    Pin: %u", this->pin_->get_pin());
  rmt.gpio_num = gpio_num_t(this->pin_->get_pin());
  ESP_LOGCONFIG(TAG, "    Clock Divider: %u", this->clock_divider_);
  rmt.clk_div = this->clock_divider_;
  rmt.mem_block_num = 1;
  rmt.rmt_mode = RMT_MODE_RX;
  if (this->filter_us_ == 0) {
    rmt.rx_config.filter_en = false;
  } else {
    ESP_LOGCONFIG(TAG, "    Filter: %u us (%u ticks)", this->filter_us_, this->from_microseconds(this->filter_us_));
    rmt.rx_config.filter_en = true;
    rmt.rx_config.filter_ticks_thresh = this->from_microseconds(this->filter_us_);
  }
  ESP_LOGCONFIG(TAG, "    Idle threshold: %u us (%u ticks)", this->idle_us_, this->from_microseconds(this->idle_us_));
  rmt.rx_config.idle_threshold = this->from_microseconds(this->idle_us_);

  rmt_config(&rmt);
}
void RemoteReceiverComponent::loop() {
  size_t len = 0;
  auto *item = (rmt_item32_t *) xRingbufferReceive(this->ringbuf_, &len, 0);
  if (item != nullptr) {
    std::vector<int32_t> v = this->decode_rmt_(item, len);
    vRingbufferReturnItem(this->ringbuf_, item);

    if (v.empty())
      return;

    RemoteReceiveData data(this, v);
    bool found_decoder = false;
    for (auto *decoder : this->decoders_) {
      if (decoder->process_(data))
        found_decoder = true;
    }

    if (!found_decoder) {
      for (auto *dumper : this->dumpers_)
        dumper->process_(data);
    }
  }
}
std::vector<int32_t> RemoteReceiverComponent::decode_rmt_(rmt_item32_t *item, size_t len) {
  bool prev_level = false;
  uint32_t prev_length = 0;
  std::vector<int32_t> data;
  int32_t multiplier = this->pin_->is_inverted() ? -1 : 1;

  for (size_t i = 0; i < len; i++) {
    if (item[i].level0 == prev_level) {
      prev_length += item[i].duration0;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          data.push_back(this->to_microseconds(prev_length) * multiplier);
        } else {
          data.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
        }
      }
      prev_level = item[i].level0;
      prev_length = item[i].duration0;
    }

    if (item[i].level1 == prev_level) {
      prev_length += item[i].duration1;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          data.push_back(this->to_microseconds(prev_length) * multiplier);
        } else {
          data.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
        }
      }
      prev_level = item[i].level1;
      prev_length = item[i].duration1;
    }
  }
  if (prev_length > 0) {
    if (prev_level) {
      data.push_back(this->to_microseconds(prev_length) * multiplier);
    } else {
      data.push_back(-int32_t(this->to_microseconds(prev_length)) * multiplier);
    }
  }
  return data;
}
#endif

#ifdef ARDUINO_ARCH_ESP8266
RemoteReceiverComponent *global_remote_receiver = nullptr;
volatile uint32_t remote_buffer_write_at_ = 0;

inline uint32_t decrement(uint32_t i, uint32_t size) {
  // unsigned integer underflow is defined behavior
  if (i - 1 > size) return size - 1;
  else return i - 1;
}

inline uint32_t increment(uint32_t i, uint32_t size) {
  if (i + 1 >= size) return 0;
  else return i + 1;
}

void RemoteReceiverComponent::gpio_intr() {
  const uint32_t now = micros();
  RemoteReceiverComponent *receiver = global_remote_receiver;
  // protect from disabled interrupts making our math invalid
  if (int(receiver->pin_->digital_read()) != remote_buffer_write_at_ % 2) {
    return;
  }
  const uint32_t last_change = receiver->buffer_[decrement(remote_buffer_write_at_, receiver->buffer_size_)];
  if (now - last_change <= receiver->filter_us_) {
    return;
  }
  receiver->buffer_[remote_buffer_write_at_++] = now;
}

void RemoteReceiverComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Remote Receiver...");
  global_remote_receiver = this;
  this->pin_->setup();
  if (this->buffer_size_ % 2 != 0) {
    // Make sure divisible by two. This way, we know that every 0bxxx0 index is a space and every 0bxxx1 index is a mark
    this->buffer_size_++;
  }
  this->buffer_ = new uint32_t[this->buffer_size_];
  // First index is a space.
  this->buffer_[0] = 0;
  this->buffer_[1] = 0;
  remote_buffer_write_at_ = 2;
  this->buffer_read_at_ = 2;
  if (this->pin_->digital_read()) {
    // Already high, increment buffer index
    remote_buffer_write_at_++;
  }
  attachInterrupt(this->pin_->get_pin(), gpio_intr, CHANGE);
}

void RemoteReceiverComponent::loop() {
  if (this->buffer_read_at_ == remote_buffer_write_at_)
    return;
  const uint32_t now = micros();
  // copy write at to stack, otherwise we might run into infinite loops
  const uint32_t write_at = remote_buffer_write_at_;
  if (now - this->buffer_[write_at] < this->idle_us_)
    // Not finished yet.
    return;

  std::vector<int32_t> v;
  // Skip first value, it's from the previous idle level
  this->buffer_read_at_ = increment(this->buffer_read_at_, this->buffer_size_);
  for (; this->buffer_read_at_ != write_at; ) {
    int32_t multiplier = this->buffer_read_at_ % 2 == 0 ? -1 : 1;
    uint32_t prev = decrement(this->buffer_read_at_, this->buffer_size_);

    int32_t delta = this->buffer_[this->buffer_read_at_] -  this->buffer_[prev];
    v.push_back(multiplier * delta);
    this->buffer_read_at_ = increment(this->buffer_read_at_, this->buffer_size_);
  }
  int32_t end_value = now - this->buffer_[write_at];
  if (write_at % 2 == 0)
    v.push_back(-end_value);
  else
    v.push_back(end_value);

  RemoteReceiveData data(this, v);
  bool found_decoder = false;
  for (auto *decoder : this->decoders_) {
    if (decoder->process_(data))
      found_decoder = true;
  }

  if (!found_decoder) {
    for (auto *dumper : this->dumpers_)
      dumper->process_(data);
  }
}

#endif

void RemoteReceiverComponent::add_decoder(RemoteReceiveDecoder *decoder) {
  this->decoders_.push_back(decoder);
}
void RemoteReceiverComponent::add_dumper(RemoteReceiveDumper *dumper) {
  this->dumpers_.push_back(dumper);
}

RemoteReceiveDecoder::RemoteReceiveDecoder(const std::string &name)
    : BinarySensor(name) {

}

bool RemoteReceiveDecoder::process_(RemoteReceiveData &data) {
  data.reset_index();
  if (this->matches(data)) {
    this->publish_state(true);
    yield();
    this->publish_state(false);
    return true;
  }
  return false;
}

void RemoteReceiveDumper::process_(RemoteReceiveData &data) {
  data.reset_index();
  this->dump(data);
}

RemoteTransmitter::RemoteTransmitter(const std::string &name)
    : Switch(name) {

}

void RemoteTransmitter::turn_on() {
  RemoteTransmitData data = this->get_data();
  this->parent_->send(data, this->send_times_, this->send_wait_);
  this->publish_state(false);
}
void RemoteTransmitter::turn_off() {
  // Turning off does nothing
  this->publish_state(false);
}
void RemoteTransmitter::set_parent(RemoteTransmitterComponent *parent) {
  this->parent_ = parent;
}

RemoteTransmitterComponent::RemoteTransmitterComponent(GPIOPin *pin)
    : RemoteControlComponentBase(pin) {

}
float RemoteTransmitterComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
#ifdef ARDUINO_ARCH_ESP32
void RemoteTransmitterComponent::setup() {
  this->configure_rmt();
  rmt_driver_install(this->channel_, 0, 0);
}

void RemoteTransmitterComponent::configure_rmt() {
  rmt_config_t c{};

  ESP_LOGCONFIG(TAG, "Configuring Remote Transmitter...");
  c.rmt_mode = RMT_MODE_TX;
  c.channel = this->channel_;
  ESP_LOGCONFIG(TAG, "    Channel: %d", this->channel_);
  c.clk_div = this->clock_divider_;
  ESP_LOGCONFIG(TAG, "    Clock divider: %u", this->clock_divider_);
  c.gpio_num = gpio_num_t(this->pin_->get_pin());
  ESP_LOGCONFIG(TAG, "    GPIO Pin: %u", this->pin_->get_pin());
  c.mem_block_num = 1;
  c.tx_config.loop_en = false;

  if (this->current_carrier_frequency_ == 0) {
    c.tx_config.carrier_en = false;
  } else {
    c.tx_config.carrier_en = true;
    c.tx_config.carrier_freq_hz = this->current_carrier_frequency_;
    ESP_LOGCONFIG(TAG, "    Carrier Frequency: %uHz", this->current_carrier_frequency_);
    c.tx_config.carrier_duty_percent = this->carrier_duty_percent_;
    ESP_LOGCONFIG(TAG, "    Carrier Duty: %u%%", this->carrier_duty_percent_);
  }

  c.tx_config.idle_output_en = true;
  if (!this->pin_->is_inverted()) {
    ESP_LOGV(TAG, "    Carrier level: HIGH");
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    ESP_LOGV(TAG, "    Idle level: LOW");
    c.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  } else {
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
    ESP_LOGV(TAG, "    Carrier level: LOW");
    c.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;
    ESP_LOGV(TAG, "    Idle level: HIGH");
  }

  rmt_config(&c);
}

void RemoteTransmitterComponent::send(const RemoteTransmitData &data, uint32_t send_times, uint32_t send_wait) {
  if (this->current_carrier_frequency_ != data.get_carrier_frequency()) {
    this->current_carrier_frequency_ = data.get_carrier_frequency();
    this->configure_rmt();
  }

  std::vector<rmt_item32_t> rmt_data;
  rmt_data.reserve((data.get_data().size() + 1) / 2);
  uint32_t rmt_i = 0;
  rmt_item32_t rmt_item;

  for (int32_t val : data.get_data()) {
    bool level = val >= 0;
    if (!level)
      val = -val;

    do {
      int32_t item = std::min(val, 32767);
      val -= item;

      if (rmt_i % 2 == 0) {
        rmt_item.level0 = static_cast<uint32_t>(level);
        rmt_item.duration0 = static_cast<uint32_t>(item);
      } else {
        rmt_item.level1 = static_cast<uint32_t>(level);
        rmt_item.duration1 = static_cast<uint32_t>(item);
        rmt_data.push_back(rmt_item);
      }
      rmt_i++;
    } while (val != 0);
  }

  if (rmt_i % 2 == 1) {
    rmt_item.level1 = 0;
    rmt_item.duration1 = 0;
    rmt_data.push_back(rmt_item);
  }

  for (uint16_t i = 0; i < send_times; i++) {
    rmt_write_items(this->channel_, rmt_data.data(), rmt_data.size(), true);
    if (i + 1 < send_times)
      delayMicroseconds(send_wait);
  }
}
#endif //ARDUINO_ARCH_ESP32

#ifdef ARDUINO_ARCH_ESP8266
void RemoteTransmitterComponent::setup() {
  this->pin_->setup();
  this->pin_->digital_write(false);
}
void RemoteTransmitterComponent::calculate_on_off_time_(uint32_t carrier_frequency,
                                                        uint32_t *on_time_period,
                                                        uint32_t *off_time_period) {
  if (carrier_frequency == 0) {
    *on_time_period = 0;
    *off_time_period = 0;
    return;
  }
  carrier_frequency = 1;
  uint32_t period = (1000000UL + carrier_frequency / 2) / carrier_frequency; // round(1000000/freq)
  period = std::max(uint32_t(1), period);
  *on_time_period = (period * this->carrier_duty_percent_) / 100;
  *off_time_period = period - *on_time_period;
}

void RemoteTransmitterComponent::send(const RemoteTransmitData &data, uint32_t send_times, uint32_t send_wait) {
  for (uint32_t i = 0; i < send_times; i++) {
    uint32_t on_time, off_time;
    this->calculate_on_off_time_(data.get_carrier_frequency(), &on_time, &off_time);
    ESP_LOGD(TAG, "Sending...");

    disable_interrupts();
    for (int32_t item : data.get_data()) {
      if (item > 0) {
        const auto length = uint32_t(item);
        this->mark_(on_time, off_time, length);
      } else {
        const auto length = uint32_t(-item);
        this->space_(length);
      }
      ESP.wdtFeed();
    }
    enable_interrupts();

    if (i + 1 < send_times) {
      uint32_t wait_ms = send_wait / 1000UL;
      if (wait_ms > 0)
        delay(wait_ms);
      delayMicroseconds(send_wait % 1000UL);
    }
  }
}
void RemoteTransmitterComponent::mark_(uint32_t on_time, uint32_t off_time, uint32_t usec) {
  if (this->carrier_duty_percent_ == 100 || (on_time == 0 && off_time == 0)) {
    this->pin_->digital_write(true);
    delay_microseconds_accurate(usec);
    this->pin_->digital_write(false);
    return;
  }

  const uint32_t start_time = micros();
  uint32_t current_time = start_time;

  while (current_time - start_time < usec) {
    const uint32_t elapsed = current_time - start_time;
    this->pin_->digital_write(true);

    delay_microseconds_accurate(std::min(on_time, usec - elapsed));
    this->pin_->digital_write(false);
    if (elapsed + on_time >= usec)
      return;

    delay_microseconds_accurate(std::min(usec - elapsed - on_time, off_time));

    current_time = micros();
  }
}
void RemoteTransmitterComponent::space_(uint32_t usec) {
  this->pin_->digital_write(false);
  delay_microseconds_accurate(usec);
}
RemoteTransmitter *RemoteTransmitterComponent::add_transmitter(RemoteTransmitter *transmitter) {
  transmitter->set_parent(this);
  this->transmitters_.push_back(transmitter);
  return transmitter;
}
void RemoteTransmitterComponent::set_carrier_duty_percent(uint8_t carrier_duty_percent) {
  this->carrier_duty_percent_ = carrier_duty_percent;
}
#endif //ARDUINO_ARCH_ESP8266

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
