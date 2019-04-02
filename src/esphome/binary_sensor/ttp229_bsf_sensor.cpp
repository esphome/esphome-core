#include "esphome/defines.h"
#ifdef USE_TTP229_BSF

#include "esphome/binary_sensor/ttp229_bsf_sensor.h"
#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.ttp229";

TTP229Channel::TTP229Channel(const std::string &name, int channel_num) : BinarySensor(name) { channel_ = channel_num; }

void TTP229Channel::process(const uint16_t *data) { this->publish_state(*data & (1 << this->channel_)); }

TTP229BSFComponent::TTP229BSFComponent(GPIOPin *sdo_pin, GPIOPin *scl_pin) : sdo_pin_(sdo_pin), scl_pin_(scl_pin) {}

void TTP229BSFComponent::setup() {
  this->sdo_pin_->setup();
  this->scl_pin_->setup();
  this->scl_pin_->digital_write(true);
  delay(10);
  uint16_t mode = read_channels_(16);
  mode &= 0x0006;
  ESP_LOGCONFIG(TAG, "readchan:%d ---- ", mode);
  switch (mode) {
    case 0:
      num_channels_ = 16;
      active_high_ = true;
      break;
    case 2:
      num_channels_ = 16;
      active_high_ = false;
      break;
    case 4:
      num_channels_ = 8;
      active_high_ = true;
      break;
    case 6:
      num_channels_ = 8;
      active_high_ = false;
      break;
  }

  ESP_LOGCONFIG(TAG, "Setting up ttp229_bsf... scl_pin:%d sdo_pin:%d channels:%d mode:%d    ---- ",
                this->scl_pin_->get_pin(), this->sdo_pin_->get_pin(), this->num_channels_, this->active_high_);
}

void TTP229BSFComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "ttp229_bsf:");
  LOG_PIN("  SCL pin: ", this->scl_pin_);
  LOG_PIN("  SDO pin: ", this->sdo_pin_);
  switch (this->error_code_) {
    case COMMUNICATION_FAILED:
      ESP_LOGE(TAG, "Communication with ttp229_bsf failed!");
      break;
    case NONE:
    default:
      break;
  }
}

float TTP229BSFComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

TTP229Channel *TTP229BSFComponent::add_channel(binary_sensor::TTP229Channel *channel) {
  this->channels_.push_back(channel);
  return channel;
}

void TTP229BSFComponent::process_(uint16_t *data) {
  for (auto *channel : this->channels_) {
    channel->process(data);
  }
}

bool TTP229BSFComponent::GetBit() {
  this->scl_pin_->digital_write(false);
  // igitalWrite(_sclPin, LOW);
  delayMicroseconds(2);  // 500KHz
  bool bitval = this->sdo_pin_->digital_read();
  // bool retVal = !digitalRead(_sdoPin);
  this->scl_pin_->digital_write(true);
  //  digitalWrite(_sclPin, HIGH);
  delayMicroseconds(2);  // 500KHz
  return bitval;
}

uint16_t TTP229BSFComponent::read_channels_(uint8_t num_channels) {
  uint16_t val = 0;
  for (uint8_t i = 0; i < num_channels; i++)
    if (GetBit())
      val |= 1 << i;
  delay(2);
  // ESP_LOGD(TAG, "val: %d", val);
  if (this->active_high_)
    val = ~val;
  return val;
}

void TTP229BSFComponent::loop() {
  // ESP_LOGD(TAG, "loop");
  this->currtouched_ = this->read_channels_(this->num_channels_);
  if (this->currtouched_ != this->lasttouched_) {
    this->process_(&currtouched_);
    ESP_LOGD(TAG, "value: %4x", this->currtouched_);
  }
  // reset touchsensor state
  this->lasttouched_ = this->currtouched_;
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TTP229_BSF