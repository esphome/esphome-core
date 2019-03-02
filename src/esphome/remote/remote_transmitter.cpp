#include "esphome/defines.h"

#ifdef USE_REMOTE_TRANSMITTER

#include "esphome/remote/remote_transmitter.h"
#include "esphome/log.h"
#include "esphome/remote/jvc.h"
#include "esphome/remote/nec.h"
#include "esphome/remote/lg.h"
#include "esphome/remote/panasonic.h"
#include "esphome/remote/remote_transmitter.h"
#include "esphome/remote/rc_switch.h"
#include "esphome/remote/rc5.h"
#include "esphome/remote/samsung.h"
#include "esphome/remote/sony.h"

#ifdef ARDUINO_ARCH_ESP32
#include <soc/rmt_struct.h>
#endif

ESPHOME_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.transmitter";

void RemoteTransmitData::mark(uint32_t length) { this->data_.push_back(length); }
void RemoteTransmitData::space(uint32_t length) { this->data_.push_back(-length); }
void RemoteTransmitData::item(uint32_t mark, uint32_t space) {
  this->mark(mark);
  this->space(space);
}
void RemoteTransmitData::reserve(uint32_t len) { this->data_.reserve(len); }
void RemoteTransmitData::set_data(std::vector<int32_t> data) {
  this->data_.clear();
  this->data_.reserve(data.size());
  for (int32_t i : data)
    this->data_.push_back(i);
}
void RemoteTransmitData::set_carrier_frequency(uint32_t carrier_frequency) {
  this->carrier_frequency_ = carrier_frequency;
}
uint32_t RemoteTransmitData::get_carrier_frequency() const { return this->carrier_frequency_; }
const std::vector<int32_t> &RemoteTransmitData::get_data() const { return this->data_; }
std::vector<int32_t>::iterator RemoteTransmitData::begin() { return this->data_.begin(); }
std::vector<int32_t>::iterator RemoteTransmitData::end() { return this->data_.end(); }
void RemoteTransmitData::reset() { this->data_.clear(); }

RemoteTransmitter::RemoteTransmitter(const std::string &name) : Switch(name) {}
void RemoteTransmitter::set_parent(RemoteTransmitterComponent *parent) { this->parent_ = parent; }
void RemoteTransmitter::set_repeat(uint32_t send_times, uint32_t send_wait) {
  this->send_times_ = send_times;
  this->send_wait_ = send_wait;
}
void RemoteTransmitter::write_state(bool state) {
  if (!state) {
    this->publish_state(false);
    return;
  }

  // write_state is called from TCP task on ESP32
  // we must defer sending until next loop execution
  this->parent_->deferred_send(this);
}
uint32_t RemoteTransmitter::get_send_times() const { return this->send_times_; }
uint32_t RemoteTransmitter::get_send_wait() const { return this->send_wait_; }

RemoteTransmitterComponent::RemoteTransmitterComponent(GPIOPin *pin) : RemoteControlComponentBase(pin) {}
float RemoteTransmitterComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
#ifdef ARDUINO_ARCH_ESP32
void RemoteTransmitterComponent::setup() {}

void RemoteTransmitterComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Remote Transmitter...");
  ESP_LOGCONFIG(TAG, "  Channel: %d", this->channel_);
  ESP_LOGCONFIG(TAG, "  Clock divider: %u", this->clock_divider_);
  LOG_PIN("  Pin: ", this->pin_);

  if (this->current_carrier_frequency_ != 0 && this->carrier_duty_percent_ != 100) {
    ESP_LOGCONFIG(TAG, "    Carrier Duty: %u%%", this->carrier_duty_percent_);
  }

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Configuring RMT driver failed: %s", esp_err_to_name(this->error_code_));
  }

  for (auto *child : this->transmitters_) {
    LOG_SWITCH("  ", "Transmitter", child);
  }
}

void RemoteTransmitterComponent::configure_rmt() {
  rmt_config_t c{};

  c.rmt_mode = RMT_MODE_TX;
  c.channel = this->channel_;
  c.clk_div = this->clock_divider_;
  c.gpio_num = gpio_num_t(this->pin_->get_pin());
  c.mem_block_num = 1;
  c.tx_config.loop_en = false;

  if (this->current_carrier_frequency_ == 0 || this->carrier_duty_percent_ == 100) {
    c.tx_config.carrier_en = false;
  } else {
    c.tx_config.carrier_en = true;
    c.tx_config.carrier_freq_hz = this->current_carrier_frequency_;
    c.tx_config.carrier_duty_percent = this->carrier_duty_percent_;
  }

  c.tx_config.idle_output_en = true;
  if (!this->pin_->is_inverted()) {
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    c.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  } else {
    c.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
    c.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;
  }

  esp_err_t error = rmt_config(&c);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->mark_failed();
    return;
  }

  if (!this->initialized_) {
    error = rmt_driver_install(this->channel_, 0, 0);
    if (error != ESP_OK) {
      this->error_code_ = error;
      this->mark_failed();
      return;
    }
    this->initialized_ = true;
  }
}

void RemoteTransmitterComponent::send_(RemoteTransmitData *data, uint32_t send_times, uint32_t send_wait) {
  if (this->is_failed())
    return;

  if (this->current_carrier_frequency_ != data->get_carrier_frequency()) {
    this->current_carrier_frequency_ = data->get_carrier_frequency();
    this->configure_rmt();
  }

  this->rmt_temp_.clear();
  this->rmt_temp_.reserve((data->get_data().size() + 1) / 2);
  uint32_t rmt_i = 0;
  rmt_item32_t rmt_item;

  for (int32_t val : data->get_data()) {
    bool level = val >= 0;
    if (!level)
      val = -val;
    val = this->from_microseconds(static_cast<uint32_t>(val));

    do {
      int32_t item = std::min(val, 32767);
      val -= item;

      if (rmt_i % 2 == 0) {
        rmt_item.level0 = static_cast<uint32_t>(level);
        rmt_item.duration0 = static_cast<uint32_t>(item);
      } else {
        rmt_item.level1 = static_cast<uint32_t>(level);
        rmt_item.duration1 = static_cast<uint32_t>(item);
        this->rmt_temp_.push_back(rmt_item);
      }
      rmt_i++;
    } while (val != 0);
  }

  if (rmt_i % 2 == 1) {
    rmt_item.level1 = 0;
    rmt_item.duration1 = 0;
    this->rmt_temp_.push_back(rmt_item);
  }

  for (uint16_t i = 0; i < send_times; i++) {
    esp_err_t error = rmt_write_items(this->channel_, this->rmt_temp_.data(), this->rmt_temp_.size(), true);
    if (error != ESP_OK) {
      ESP_LOGW(TAG, "rmt_write_items failed: %s", esp_err_to_name(error));
      this->status_set_warning();
    } else {
      this->status_clear_warning();
    }
    if (i + 1 < send_times) {
      delay(send_wait / 1000UL);
      delayMicroseconds(send_wait % 1000UL);
    }
  }
}
#endif  // ARDUINO_ARCH_ESP32

#ifdef ARDUINO_ARCH_ESP8266
void RemoteTransmitterComponent::setup() {
  this->pin_->setup();
  this->pin_->digital_write(false);
}

void RemoteTransmitterComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Remote Transmitter...");
  ESP_LOGCONFIG(TAG, "  Carrier Duty: %u%%", this->carrier_duty_percent_);
  LOG_PIN("  Pin: ", this->pin_);

  for (auto *child : this->transmitters_) {
    LOG_SWITCH("  ", "Transmitter", child);
  }
}

void RemoteTransmitterComponent::calculate_on_off_time_(uint32_t carrier_frequency, uint32_t *on_time_period,
                                                        uint32_t *off_time_period) {
  if (carrier_frequency == 0) {
    *on_time_period = 0;
    *off_time_period = 0;
    return;
  }
  uint32_t period = (1000000UL + carrier_frequency / 2) / carrier_frequency;  // round(1000000/freq)
  period = std::max(uint32_t(1), period);
  *on_time_period = (period * this->carrier_duty_percent_) / 100;
  *off_time_period = period - *on_time_period;
}

void RemoteTransmitterComponent::send_(RemoteTransmitData *data, uint32_t send_times, uint32_t send_wait) {
  for (uint32_t i = 0; i < send_times; i++) {
    uint32_t on_time, off_time;
    this->calculate_on_off_time_(data->get_carrier_frequency(), &on_time, &off_time);
    ESP_LOGD(TAG, "Sending remote code...");

    ESP.wdtFeed();
    disable_interrupts();
    for (int32_t item : *data) {
      if (item > 0) {
        const auto length = uint32_t(item);
        this->mark_(on_time, off_time, length);
      } else {
        const auto length = uint32_t(-item);
        this->space_(length);
      }
      feed_wdt();
    }
    enable_interrupts();

    if (i + 1 < send_times) {
      delay(send_wait / 1000UL);
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
#endif  // ARDUINO_ARCH_ESP8266

RemoteTransmitter *RemoteTransmitterComponent::add_transmitter(RemoteTransmitter *transmitter) {
  transmitter->set_parent(this);
  this->transmitters_.push_back(transmitter);
  return transmitter;
}
void RemoteTransmitterComponent::set_carrier_duty_percent(uint8_t carrier_duty_percent) {
  this->carrier_duty_percent_ = carrier_duty_percent;
}
RemoteTransmitterComponent::TransmitCall RemoteTransmitterComponent::transmit() { return TransmitCall(this); }
void RemoteTransmitterComponent::deferred_send(RemoteTransmitter *a_switch) {
  this->defer([this, a_switch]() {
    a_switch->publish_state(true);
    this->temp_.reset();
    a_switch->to_data(&this->temp_);
    this->send_(&this->temp_, a_switch->get_send_times(), a_switch->get_send_wait());
    a_switch->publish_state(false);
  });
}

void RemoteTransmitterComponent::TransmitCall::perform() {
  this->parent_->send_(&this->parent_->temp_, this->send_times_, this->send_wait_);
}
RemoteTransmitterComponent::TransmitCall::TransmitCall(RemoteTransmitterComponent *parent) : parent_(parent) {
  this->get_data()->reset();
}
RemoteTransmitData *RemoteTransmitterComponent::TransmitCall::get_data() { return &this->parent_->temp_; }
void RemoteTransmitterComponent::TransmitCall::set_jvc(uint32_t data) { encode_jvc(this->get_data(), data); }
void RemoteTransmitterComponent::TransmitCall::set_lg(uint32_t data, uint8_t nbits) {
  encode_lg(this->get_data(), data, nbits);
}
void RemoteTransmitterComponent::TransmitCall::set_nec(uint16_t address, uint16_t command) {
  encode_nec(this->get_data(), address, command);
}
void RemoteTransmitterComponent::TransmitCall::set_panasonic(uint16_t address, uint32_t command) {
  encode_panasonic(this->get_data(), address, command);
}
void RemoteTransmitterComponent::TransmitCall::set_raw(std::vector<int32_t> data) { this->get_data()->set_data(data); }
void RemoteTransmitterComponent::TransmitCall::set_rc5(uint8_t address, uint8_t command, bool toggle) {
  encode_rc5(this->get_data(), address, command, toggle);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_raw(uint32_t code, uint8_t nbits,
                                                                 RCSwitchProtocol protocol) {
  encode_rc_switch_raw(this->get_data(), code, nbits, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_raw(const char *code, RCSwitchProtocol protocol) {
  encode_rc_switch_raw(this->get_data(), code, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_raw_tristate(const char *code, RCSwitchProtocol protocol) {
  encode_rc_switch_raw_tristate(this->get_data(), code, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_type_a(uint8_t switch_group, uint8_t switch_device,
                                                                    bool state, RCSwitchProtocol protocol) {
  encode_rc_switch_type_a(this->get_data(), switch_group, switch_device, state, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_type_a(const char *switch_group, const char *switch_device,
                                                                    bool state, RCSwitchProtocol protocol) {
  encode_rc_switch_type_a(this->get_data(), switch_group, switch_device, state, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_type_b(uint8_t address, uint8_t channel, bool state,
                                                                    RCSwitchProtocol protocol) {
  encode_rc_switch_type_b(this->get_data(), address, channel, state, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_type_c(uint8_t family, uint8_t group, uint8_t device,
                                                                    bool state, RCSwitchProtocol protocol) {
  encode_rc_switch_type_c(this->get_data(), family, group, device, state, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_type_c(char family, uint8_t group, uint8_t device,
                                                                    bool state, RCSwitchProtocol protocol) {
  encode_rc_switch_type_c(this->get_data(), family, group, device, state, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_type_d(uint8_t group, uint8_t device, bool state,
                                                                    RCSwitchProtocol protocol) {
  encode_rc_switch_type_d(this->get_data(), group, device, state, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_rc_switch_type_d(char group, uint8_t device, bool state,
                                                                    RCSwitchProtocol protocol) {
  encode_rc_switch_type_d(this->get_data(), group, device, state, protocol);
}
void RemoteTransmitterComponent::TransmitCall::set_samsung(uint32_t data) { encode_samsung(this->get_data(), data); }
void RemoteTransmitterComponent::TransmitCall::set_sony(uint32_t data, uint8_t nbits) {
  encode_sony(this->get_data(), data, nbits);
}

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE_TRANSMITTER
