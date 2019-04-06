#include "esphome/defines.h"
#ifdef USE_TTP229_BSF

#include "esphome/binary_sensor/ttp229_bsf_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.ttp229_bsf";

TTP229BSFChannel::TTP229BSFChannel(const std::string &name, int channel_num) : BinarySensor(name) {
  channel_ = channel_num;
}

void TTP229BSFChannel::process(const uint16_t data) { this->publish_state(data & (1 << this->channel_)); }

TTP229BSFComponent::TTP229BSFComponent(GPIOPin *sdo_pin, GPIOPin *scl_pin) : sdo_pin_(sdo_pin), scl_pin_(scl_pin) {}

void TTP229BSFComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ttp229_bsf... ");
  this->sdo_pin_->setup();
  this->scl_pin_->setup();
  this->scl_pin_->digital_write(true);
  delay(2);
}

void TTP229BSFComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "ttp229_bsf:");
  LOG_PIN("  SCL pin: ", this->scl_pin_);
  LOG_PIN("  SDO pin: ", this->sdo_pin_);
}

float TTP229BSFComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

TTP229BSFChannel *TTP229BSFComponent::add_channel(binary_sensor::TTP229BSFChannel *channel) {
  this->channels_.push_back(channel);
  return channel;
}

bool TTP229BSFComponent::get_bit_() {
  this->scl_pin_->digital_write(false);
  delayMicroseconds(2);  // 500KHz
  bool bitval = this->sdo_pin_->digital_read();
  this->scl_pin_->digital_write(true);
  delayMicroseconds(2);  // 500KHz
  return bitval;
}

uint16_t TTP229BSFComponent::read_data_(uint8_t num_bits) {
  uint16_t val = 0;
  for (uint8_t i = 0; i < num_bits; i++)
    if (get_bit_())
      val |= 1 << i;
  return ~val;
}

void TTP229BSFComponent::loop() {
  uint16_t touched = 0;
  touched = this->read_data_(16);
  for (auto *channel : this->channels_) {
    channel->process(touched);
  }
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TTP229_BSF
