#include "esphome/defines.h"

#ifdef USE_RDM6300

#include "esphome/binary_sensor/rdm6300.h"
#include "esphome/application.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.rdm6300";
static const uint8_t RDM6300_START_BYTE = 0x02;
static const uint8_t RDM6300_END_BYTE = 0x03;
static const int8_t RDM6300_STATE_WAITING_FOR_START = -1;

void RDM6300Component::loop() {
  while (this->available() > 0) {
    uint8_t data;
    if (!this->read_byte(&data)) {
      ESP_LOGW(TAG, "Reading data from RDM6300 failed!");
      this->status_set_warning();
      return;
    }

    if (this->read_state_ == RDM6300_STATE_WAITING_FOR_START) {
      if (data == RDM6300_START_BYTE) {
        this->read_state_ = 0;
      } else {
        // Not start byte, probably not synced up correctly.
      }
    } else if (this->read_state_ < 12) {
      uint8_t value = (data > '9') ? data - '7' : data - '0';
      if (this->read_state_ % 2 == 0) {
        this->buffer_[this->read_state_ / 2] = value << 4;
      } else {
        this->buffer_[this->read_state_ / 2] += value;
      }
      this->read_state_++;
    } else if (data != RDM6300_END_BYTE) {
      ESP_LOGW(TAG, "Invalid end byte from RDM6300!");
      this->read_state_ = RDM6300_STATE_WAITING_FOR_START;
    } else {
      uint8_t checksum = 0;
      for (uint8_t i = 0; i < 5; i++)
        checksum ^= this->buffer_[i];
      this->read_state_ = RDM6300_STATE_WAITING_FOR_START;
      if (checksum != this->buffer_[5]) {
        ESP_LOGW(TAG, "Checksum from RDM6300 doesn't match! (0x%02X!=0x%02X)", checksum, this->buffer_[5]);
      } else {
        // Valid data
        this->status_clear_warning();
        const uint32_t result = (uint32_t(this->buffer_[1]) << 24) | (uint32_t(this->buffer_[2]) << 16) |
                                (uint32_t(this->buffer_[3]) << 8) | this->buffer_[4];
        bool report = result != last_id_;
        for (auto *card : this->cards_) {
          if (card->process(result)) {
            report = false;
          }
        }

        if (report) {
          ESP_LOGD(TAG, "Found new tag with ID %u", result);
        }
      }
    }
  }
}
RDM6300BinarySensor *RDM6300Component::make_card(const std::string &name, uint32_t id) {
  auto *card = new RDM6300BinarySensor(name, id);
  this->cards_.push_back(card);
  return card;
}
float RDM6300Component::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
RDM6300Component::RDM6300Component(UARTComponent *parent) : Component(), UARTDevice(parent) {}

RDM6300BinarySensor::RDM6300BinarySensor(const std::string &name, uint32_t id) : BinarySensor(name), id_(id) {}
bool RDM6300BinarySensor::process(uint32_t id) {
  if (this->id_ == id) {
    this->publish_state(true);
    yield();
    this->publish_state(false);
    return true;
  }
  return false;
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_RDM6300
