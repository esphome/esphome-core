//
// Created by Otto Winter on 01.12.17.
//
// ESP8266 code based on IRRemote8266: https://github.com/markszabo/IRremoteESP8266
// DataTransmitter from_* based on IRRemote: https://github.com/z3t0/Arduino-IRremote


#include "esphomelib/switch_/ir_transmitter_component.h"

#include <cstdlib>

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"

// #ifdef USE_IR_TRANSMITTER

#ifdef ARDUINO_ARCH_ESP32
#include <esp32-hal.h>
#include <driver/rmt.h>
#include <soc/rmt_struct.h>
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.ir_transmitter";

using namespace esphomelib::switch_::ir;

namespace ir {

void SendData::mark(uint16_t duration_us) {
  this->data.push_back(static_cast<short>(duration_us));
}
void SendData::space(uint16_t duration_us) {
  this->data.push_back(static_cast<short>(duration_us * -1));
}
void SendData::add_item(uint16_t high_us, uint16_t low_us) {
  this->mark(high_us);
  this->space(low_us);
}
SendData SendData::from_raw(std::vector<int> &raw_data, uint32_t carrier_frequency) {
  SendData send_data{};
  send_data.carrier_frequency = carrier_frequency;
  send_data.data.reserve(raw_data.size());

  for (auto &&x : raw_data) {
    if (x >= 0)
      send_data.mark(uint16_t(x));
    else
      send_data.space(-x);
  }
  return send_data;
}

SendData SendData::from_nec(uint16_t address, uint16_t command) {
  SendData send_data{};
  send_data.carrier_frequency = nec::CARRIER_FREQUENCY_HZ;
  send_data.data.reserve(68);

  send_data.add_item(nec::HEADER_HIGH_US, nec::HEADER_LOW_US);

  for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
    if (address & mask)
      send_data.add_item(nec::BIT_HIGH_US, nec::BIT_ONE_LOW_US);
    else
      send_data.add_item(nec::BIT_HIGH_US, nec::BIT_ZERO_LOW_US);
  }

  for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
    if (command & mask)
      send_data.add_item(nec::BIT_HIGH_US, nec::BIT_ONE_LOW_US);
    else
      send_data.add_item(nec::BIT_HIGH_US, nec::BIT_ZERO_LOW_US);
  }

  send_data.add_item(nec::BIT_HIGH_US, 0);
  return send_data;
}
SendData SendData::from_lg(uint32_t data, uint8_t nbits) {
  SendData send_data{};
  send_data.carrier_frequency = lg::CARRIER_FREQUENCY_HZ;
  send_data.data.reserve(static_cast<unsigned int>(2 + nbits * 2));

  send_data.add_item(lg::HEADER_HIGH_US, lg::HEADER_LOW_US);

  for (uint32_t mask = 1UL << (nbits - 1); mask; mask >>= 1) {
    if (data & mask)
      send_data.add_item(lg::BIT_HIGH_US, lg::BIT_ONE_LOW_US);
    else
      send_data.add_item(lg::BIT_HIGH_US, lg::BIT_ZERO_LOW_US);
  }
  return send_data;
}
SendData SendData::from_sony(uint32_t data, uint8_t nbits) {
  SendData send_data{};
  send_data.carrier_frequency = sony::CARRIER_FREQUENCY_HZ;
  send_data.data.reserve(static_cast<unsigned int>(2 + nbits * 2));

  send_data.add_item(sony::HEADER_HIGH_US, sony::HEADER_LOW_US);

  for (uint32_t mask = 1UL << (nbits - 1); mask; mask >>= 1) {
    if (data & mask)
      send_data.add_item(sony::BIT_ONE_HIGH_US, sony::BIT_LOW_US);
    else
      send_data.add_item(sony::BIT_ZERO_HIGH_US, sony::BIT_LOW_US);
  }
  return send_data;
}

SendData SendData::from_panasonic(uint16_t address, uint32_t data) {
  SendData send_data{};
  send_data.carrier_frequency = panasonic::CARRIER_FREQUENCY_HZ;
  send_data.data.reserve(100);

  send_data.add_item(panasonic::HEADER_HIGH_US, panasonic::HEADER_LOW_US);

  uint32_t mask;
  for (mask = 1UL << 15; mask; mask >>= 1) {
    if (address & mask)
      send_data.add_item(panasonic::BIT_HIGH_US, panasonic::BIT_ONE_LOW_US);
    else
      send_data.add_item(panasonic::BIT_HIGH_US, panasonic::BIT_ZERO_LOW_US);
  }

  for (mask = 1UL << 31; mask; mask >>= 1) {
    if (data & mask) {
      send_data.add_item(panasonic::BIT_HIGH_US, panasonic::BIT_ONE_LOW_US);
    } else {
      send_data.add_item(panasonic::BIT_HIGH_US, panasonic::BIT_ZERO_LOW_US);
    }
  }

  send_data.add_item(panasonic::BIT_HIGH_US, 0);
  return send_data;
}
SendData SendData::repeat(uint16_t times, uint16_t wait_us) {
  this->repeat_times = times;
  this->repeat_wait = wait_us;
  return *this;
}
uint32_t SendData::total_length_ms() const {
  uint32_t total = 0;
  for (auto &&v : this->data)
    total += std::abs(v);

  uint32_t result = (this->repeat_times - 1) * (total + this->repeat_wait);
  result += total;

  return result / 1000;
}

#ifdef ARDUINO_ARCH_ESP32
std::vector<rmt_item32_t> SendData::get_rmt_data(uint16_t ticks_for_10_us) {
  std::vector<rmt_item32_t> rmt_data((this->data.size() + 1) / 2, rmt_item32_t {});

  for (uint32_t i = 0; i < this->data.size(); i++) {
    int16_t x = this->data[i];
    bool level = x >= 0;
    auto duration = uint16_t(fabsf(x) / 10 * ticks_for_10_us);

    rmt_item32_t *item = &rmt_data[i / 2];
    if (i % 2 == 0) {
      item->level0 = static_cast<uint32_t>(level);
      item->duration0 = duration;
    } else {
      item->level1 = static_cast<uint32_t>(level);
      item->duration1 = duration;
    }
  }
  ESP_LOGV(TAG, "RMT Data Length: %u ms", this->total_length_ms());

  return rmt_data;
}
#endif


} // namespace ir

#ifdef ARDUINO_ARCH_ESP32
void IRTransmitterComponent::setup() {
  this->configure_rmt();
  rmt_driver_install(this->channel_, 0, 0);
}
void IRTransmitterComponent::configure_rmt() {
  rmt_config_t c{};

  ESP_LOGCONFIG(TAG, "Configuring RMT TX...");
  c.rmt_mode = RMT_MODE_TX;
  c.channel = this->channel_;
  ESP_LOGCONFIG(TAG, "    Channel: %d", this->channel_);
  c.clk_div = this->clock_divider_;
  ESP_LOGCONFIG(TAG, "    Clock divider: %u", this->clock_divider_);
  ESP_LOGV(TAG, "     -> ticks for 10 µs: %u", this->get_ticks_for_10_us());
  c.gpio_num = gpio_num_t(this->pin_->get_pin());
  ESP_LOGCONFIG(TAG, "    GPIO Pin: %u", this->pin_->get_pin());
  c.mem_block_num = 1;
  c.tx_config.loop_en = false;
  c.tx_config.carrier_freq_hz = this->last_carrier_frequency_;
  ESP_LOGCONFIG(TAG, "    Carrier Frequency: %u", this->last_carrier_frequency_);
  c.tx_config.carrier_duty_percent = this->carrier_duty_percent_;
  ESP_LOGCONFIG(TAG, "    Carrier Duty: %u%%", this->carrier_duty_percent_);
  c.tx_config.carrier_en = true;
  c.tx_config.idle_output_en = true;
  if (!this->pin_->is_inverted()) {
    ESP_LOGV(TAG, "    Carrier level: HIGH");
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    ESP_LOGV(TAG, "    Idle level: LOW");
    c.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  } else {
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
    ESP_LOGV(TAG, "    Carrier level: HIGH");
    c.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;
    ESP_LOGV(TAG, "    Idle level: LOW");
  }
  ESP_LOGCONFIG(TAG, "    Applying...");

  rmt_config(&c);
}

uint16_t IRTransmitterComponent::get_ticks_for_10_us() {
  return static_cast<uint16_t>(BASE_CLOCK_HZ / this->clock_divider_ / 100000);
}
void IRTransmitterComponent::send(ir::SendData &send_data) {
  this->require_carrier_frequency(send_data.carrier_frequency);

  std::vector<rmt_item32_t> v = send_data.get_rmt_data(this->get_ticks_for_10_us());
  for (uint16_t i = 0; i < send_data.repeat_times - 1; i++) {
    rmt_write_items(this->channel_, v.data(), v.size(), true);
    delayMicroseconds(send_data.repeat_wait);
  }
  rmt_write_items(this->channel_, v.data(), v.size(), true);
}
void IRTransmitterComponent::require_carrier_frequency(uint32_t carrier_frequency) {
  if (this->last_carrier_frequency_ == carrier_frequency)
    return;
  this->last_carrier_frequency_ = carrier_frequency;

  this->configure_rmt();
}
void IRTransmitterComponent::set_channel(rmt_channel_t channel) {
  assert(channel < RMT_CHANNEL_MAX);
  this->channel_ = channel;
}
void IRTransmitterComponent::set_clock_divider(uint8_t clock_divider) {
  this->clock_divider_ = clock_divider;
}

rmt_channel_t next_rmt_channel = RMT_CHANNEL_0;
#endif

#ifdef ARDUINO_ARCH_ESP8266
void IRTransmitterComponent::setup() {

}
#endif

IRTransmitterComponent::IRTransmitterComponent(GPIOPin *pin,
                                               uint8_t carrier_duty_percent)
    : carrier_duty_percent_(carrier_duty_percent),
      pin_(pin) {
#ifdef ARDUINO_ARCH_ESP32
  this->set_channel(select_next_rmt_channel());
#endif
}

#ifdef ARDUINO_ARCH_ESP8266
void IRTransmitterComponent::calculate_on_off_time_(uint32_t carrier_frequency,
                                                    uint32_t *on_time_period,
                                                    uint32_t *off_time_period) {
  if (carrier_frequency == 0)
    carrier_frequency = 1;
  uint32_t period = (1000000UL + carrier_frequency / 2) / carrier_frequency; // round(1000000/freq)
  period = std::max(uint32_t(1), period);
  *on_time_period = (period * this->carrier_duty_percent_) / 100;
  *off_time_period = period - *on_time_period;
}
void IRTransmitterComponent::delay_microseconds_accurate_(uint32_t usec) {
  if (usec == 0)
    return;

  if (usec <= 16383UL) {
    delayMicroseconds(usec);
  } else {
    delay(usec / 1000UL);
    delayMicroseconds(usec % 1000UL);
  }
}
void IRTransmitterComponent::send(ir::SendData &send_data) {
  // maintain timing across marks/spaces
  for (uint16_t i = 0; i < send_data.repeat_times; i++) {
    uint32_t on_time, off_time;
    this->calculate_on_off_time_(send_data.carrier_frequency, &on_time, &off_time);

    enable_interrupts();
    for (int16_t item : send_data.data) {
      if (item > 0) {
        const auto length = static_cast<uint16_t>(item);
        this->mark_(on_time, off_time, length);
      } else {
        const auto length = static_cast<uint16_t>(-item);
        this->space_(length);
      }
      ESP.wdtFeed();
    }
    disable_interrupts();

    if (i + 1 < send_data.repeat_times) {
      uint32_t wait_ms = send_data.repeat_wait / 1000UL;
      if (wait_ms > 0)
        delay(wait_ms);
      delayMicroseconds(send_data.repeat_wait % 1000UL);
    }
  }
}
void IRTransmitterComponent::mark_(uint32_t on_time, uint32_t off_time, uint32_t usec) {
  if (this->carrier_duty_percent_ == 100) {
    this->pin_->digital_write(true);
    this->delay_microseconds_accurate_(usec);
    this->pin_->digital_write(false);
    return;
  }

  const uint32_t start_time = micros();
  uint32_t current_time = start_time;

  while (current_time - start_time < usec) {
    const uint32_t elapsed = current_time - start_time;
    this->pin_->digital_write(true);

    this->delay_microseconds_accurate_(std::min(on_time, usec - elapsed));
    this->pin_->digital_write(false);
    if (elapsed + on_time >= usec)
      return;

    this->delay_microseconds_accurate_(std::min(usec - elapsed - on_time, off_time));

    current_time = micros();
  }
}
void IRTransmitterComponent::space_(uint32_t usec) {
  this->pin_->digital_write(false);
  this->delay_microseconds_accurate_(usec);
}
#endif

float IRTransmitterComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void IRTransmitterComponent::set_carrier_duty_percent(uint8_t carrier_duty_percent) {
  this->carrier_duty_percent_ = carrier_duty_percent;
}
IRTransmitterComponent::DataTransmitter *IRTransmitterComponent::create_transmitter(const std::string &name,
                                                                                    const ir::SendData &send_data) {
  return new DataTransmitter(name, send_data, this);
}

IRTransmitterComponent::DataTransmitter::DataTransmitter(const std::string &name,
                                                         const ir::SendData &send_data,
                                                         IRTransmitterComponent *parent)
    : Switch(name), send_data_(send_data), parent_(parent) {}

void IRTransmitterComponent::DataTransmitter::turn_on() {
  this->parent_->send(this->send_data_);
  this->publish_state(false);
}
void IRTransmitterComponent::DataTransmitter::turn_off() {
  // Turning off does nothing
  this->publish_state(false);
}
std::string IRTransmitterComponent::DataTransmitter::icon() {
  return "mdi:remote";
}

} // namespace output

ESPHOMELIB_NAMESPACE_END

// #endif //USE_IR_TRANSMITTER
