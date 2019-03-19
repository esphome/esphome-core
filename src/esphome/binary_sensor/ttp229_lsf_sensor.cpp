#include "esphome/defines.h"
#ifdef USE_TTP229_LSF

#include "esphome/binary_sensor/ttp229_lsf_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.ttp229";

TTP229Channel::TTP229Channel(const std::string &name, int channel_num) : BinarySensor(name) { channel_ = channel_num; }

void TTP229Channel::process(const uint16_t *data, const uint16_t *last_data) {
  if ((*data & (1 << this->channel_)) && !(*last_data & (1 << this->channel_))) {
    this->publish_state(true);
  }
  if (!(*data & (1 << this->channel_)) && (*last_data & (1 << this->channel_))) {
    this->publish_state(false);
  }
}

TTP229LSFComponent::TTP229LSFComponent(I2CComponent *parent, uint8_t address) : I2CDevice(parent, address) {}

void TTP229LSFComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ttp229...");
  if (!this->parent_->raw_request_from(this->address_, 2)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
}

void TTP229LSFComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "ttp229:");
  LOG_I2C_DEVICE(this);
  switch (this->error_code_) {
    case COMMUNICATION_FAILED:
      ESP_LOGE(TAG, "Communication with ttp229 failed!");
      break;
    case WRONG_CHIP_STATE:
      ESP_LOGE(TAG, "ttp229 has wrong default value for CONFIG2?");
      break;
    case NONE:
    default:
      break;
  }
}

float TTP229LSFComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

TTP229Channel *TTP229LSFComponent::add_channel(binary_sensor::TTP229Channel *channel) {
  this->channels_.push_back(channel);
  return channel;
}

void TTP229LSFComponent::process_(uint16_t *data, uint16_t *last_data) {
  for (auto *channel : this->channels_) {
    channel->process(data, last_data);
  }
}

uint8_t TTP229LSFComponent::byte_reverse_(uint8_t byte) {
  uint8_t byte_rev = 0;
  for (int i = 0; i < 8; i++)
    byte_rev |= ((byte >> i) & 1) << (7 - i);
  return byte_rev;
}

uint16_t TTP229LSFComponent::read_channels_() {
  uint16_t val = 0;
  this->parent_->raw_receive_16(this->address_, &val, 1);
  // bit order in lsb_byte needs to be reversed
  uint8_t lsb = TTP229LSFComponent::byte_reverse_(val >> 8);
  // bit order in msb_byte needs to be reversed
  uint8_t msb = TTP229LSFComponent::byte_reverse_((uint8_t) val);
  val = ((uint16_t) msb) << 8;
  val |= lsb;
  return val;
}

void TTP229LSFComponent::loop() {
  this->currtouched_ = this->read_channels_();
  if (this->currtouched_ != this->lasttouched_) {
    ESP_LOGD(TAG, "    Received 0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN " (0x%04X)",
             BYTE_TO_BINARY(this->currtouched_ >> 8), BYTE_TO_BINARY(this->currtouched_), this->currtouched_);
    this->process_(&currtouched_, &lasttouched_);
  }
  // reset touchsensor state
  this->lasttouched_ = this->currtouched_;
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TTP229_LSF
