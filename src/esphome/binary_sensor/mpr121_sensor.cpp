#include "esphome/defines.h"
#ifdef USE_MPR121

#include "esphome/binary_sensor/mpr121_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.mpr121";

MPR121Channel::MPR121Channel(const std::string &name, int channel_num) : BinarySensor(name) { channel_ = channel_num; }

void MPR121Channel::process(const uint16_t *data, const uint16_t *last_data) {
  if ((*data & (1 << this->channel_)) && !(*last_data & (1 << this->channel_))) {
    this->publish_state(true);
  }
  if (!(*data & (1 << this->channel_)) && (*last_data & (1 << this->channel_))) {
    this->publish_state(false);
  }
}

MPR121Component::MPR121Component(I2CComponent *parent, uint8_t address) : I2CDevice(parent, address) {}

void MPR121Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MPR121...");
  // soft reset device
  this->write_byte(MPR121_SOFTRESET, 0x63);
  delay(100);
  if (!this->write_byte(MPR121_ECR, 0x0)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  // set touch sensitivity for all 12 channels
  for (uint8_t i = 0; i < 12; i++) {
    this->write_byte(MPR121_TOUCHTH_0 + 2 * i, 12);
    this->write_byte(MPR121_RELEASETH_0 + 2 * i, 6);
  }
  this->write_byte(MPR121_MHDR, 0x01);
  this->write_byte(MPR121_NHDR, 0x01);
  this->write_byte(MPR121_NCLR, 0x0E);
  this->write_byte(MPR121_FDLR, 0x00);

  this->write_byte(MPR121_MHDF, 0x01);
  this->write_byte(MPR121_NHDF, 0x05);
  this->write_byte(MPR121_NCLF, 0x01);
  this->write_byte(MPR121_FDLF, 0x00);

  this->write_byte(MPR121_NHDT, 0x00);
  this->write_byte(MPR121_NCLT, 0x00);
  this->write_byte(MPR121_FDLT, 0x00);

  this->write_byte(MPR121_DEBOUNCE, 0);
  // default, 16uA charge current
  this->write_byte(MPR121_CONFIG1, 0x10);
  // 0.5uS encoding, 1ms period
  this->write_byte(MPR121_CONFIG2, 0x20);
  // start with first 5 bits of baseline tracking
  this->write_byte(MPR121_ECR, 0x8F);
}

void MPR121Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MPR121:");
  LOG_I2C_DEVICE(this);
  switch (this->error_code_) {
    case COMMUNICATION_FAILED:
      ESP_LOGE(TAG, "Communication with MPR121 failed!");
      break;
    case WRONG_CHIP_STATE:
      ESP_LOGE(TAG, "MPR121 has wrong default value for CONFIG2?");
      break;
    case NONE:
    default:
      break;
  }
}

float MPR121Component::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

MPR121Channel *MPR121Component::add_channel(binary_sensor::MPR121Channel *channel) {
  this->channels_.push_back(channel);
  return channel;
}

void MPR121Component::process_(uint16_t *data, uint16_t *last_data) {
  for (auto *channel : this->channels_) {
    channel->process(data, last_data);
  }
}

uint16_t MPR121Component::read_mpr121_channels_() {
  uint16_t val = 0;
  this->read_byte_16(MPR121_TOUCHSTATUS_L, &val);
  uint8_t lsb = val >> 8;
  uint8_t msb = val;
  val = ((uint16_t) msb) << 8;
  val |= lsb;
  return val;
}

void MPR121Component::loop() {
  this->currtouched_ = this->read_mpr121_channels_();
  if (this->currtouched_ != this->lasttouched_) {
    this->process_(&currtouched_, &lasttouched_);
  }
  // reset touchsensor state
  this->lasttouched_ = this->currtouched_;
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPR121
