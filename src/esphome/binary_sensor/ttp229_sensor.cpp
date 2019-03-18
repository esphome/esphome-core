#include "esphome/defines.h"
#ifdef USE_TTP229

#include "esphome/binary_sensor/ttp229_sensor.h"
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

TTP229Component::TTP229Component(I2CComponent *parent, uint8_t address) : I2CDevice(parent, address) {}

void TTP229Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ttp229...");
  if (!this->write_byte(0x00, 0x0)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
}

void TTP229Component::dump_config() {
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

float TTP229Component::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

TTP229Channel *TTP229Component::add_channel(binary_sensor::TTP229Channel *channel) {
  this->channels_.push_back(channel);
  return channel;
}

void TTP229Component::process_(uint16_t *data, uint16_t *last_data) {
  for (auto *channel : this->channels_) {
    channel->process(data, last_data);
  }
}

uint16_t TTP229Component::read_channels_() {
  uint16_t val = 0;
  this->read_byte_16(0x00, &val);
  uint8_t lsb = val >> 8;
  uint8_t msb = val;
  val = ((uint16_t) msb) << 8;
  val |= lsb;
  return val;
}

void TTP229Component::loop() {
  this->currtouched_ = this->read_channels_();
  if (this->currtouched_ != this->lasttouched_) {
    this->process_(&currtouched_, &lasttouched_);
  }
  // reset touchsensor state
  this->lasttouched_ = this->currtouched_;
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TTP229
