#include "esphome/defines.h"
#ifdef USE_TTP229_LSF

#include "esphome/binary_sensor/ttp229_lsf_sensor.h"
#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.ttp229";

TTP229Channel::TTP229Channel(const std::string &name, int channel_num) : BinarySensor(name), channel_(channel_num) {}

void TTP229Channel::process(uint16_t data) { this->publish_state(data & (1 << this->channel_)); }

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
      ESP_LOGE(TAG, "Communication with TTP229 failed!");
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

void TTP229LSFComponent::loop() {
  uint16_t touched = 0;
  if (!this->parent_->raw_receive_16(this->address_, &touched, 1)) {
    this->status_set_warning();
    return;
  }
  this->status_clear_warning();
  touched = reverse_bits_16(touched);
  for (auto *channel : this->channels_) {
    channel->process(touched);
  }
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TTP229_LSF
