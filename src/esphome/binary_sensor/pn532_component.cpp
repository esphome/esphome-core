#include "esphome/defines.h"

#ifdef USE_PN532

#include "esphome/binary_sensor/pn532_component.h"
#include "esphome/log.h"
#include "esphome/application.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "pn532";

static const uint8_t PN532_NACK[] = {0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};
static const uint8_t PN532_ACK[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};

void format_uid(char *buf, uint8_t *uid, uint8_t uid_length) {
  int offset = 0;
  for (uint8_t i = 0; i < uid_length; i++) {
    const char *format = "%02X";
    if (i + 1 < uid_length)
      format = "%02X-";
    offset += sprintf(buf + offset, format, uid[i]);
  }
}

void PN532Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PN532...");
  this->spi_setup();

  // Let the chip start up by enabling it for some time
  this->cs_->digital_write(false);
  delay(10);

  // Send dummy command to sync up with PN532 but ignore result
  this->buffer_[0] = 0x02; // GET_FIRMWARE_VERSION
  this->pn532_write_command_check_ack_(1, true);

  // setup secure access module
  this->buffer_[0] = 0x14;
  this->buffer_[1] = 0x01; // normal mode
  // The length of a scan as a multiple of 50ms
  this->buffer_[2] = std::min(255u, this->update_interval_ / 50);
  this->buffer_[3] = 0x01; // use IRQ pin, actually we don't need it but can't hurt either

  if (!this->pn532_write_command_check_ack_(4)) {
    this->error_code_ = WRITING_SAM_COMMAND_FAILED;
    this->mark_failed();
    return;
  }

  this->pn532_read_data_(8);
  if (this->buffer_[5] != 0x15) {
    this->error_code_ = READING_SAM_COMMAND_FAILED;
    this->mark_failed();
    return;
  }
}

void PN532Component::update() {
  this->buffer_[0] = 0x4A; // INLISTPASSIVETARGET
  this->buffer_[1] = 0x01; // max 1 cards at once
  this->buffer_[2] = 0x00; // Baud rate ISO14443A
  if (!this->pn532_write_command_check_ack_(3)) {
    ESP_LOGW(TAG, "Requesting TAG read failed!");
    this->status_set_warning();
    return;
  }
  this->status_clear_warning();
  this->requested_read_ = true;
}
void PN532Component::loop() {
  if (!this->requested_read_ || !this->is_ready())
    return;

  this->pn532_read_data_(20);
  this->requested_read_ = false;

  if (this->buffer_[7] != 1) {
    // No tags found
    return;
  }

  ESP_LOGV(TAG, "ATQA: 0x%04X, SAK: 0x%02X", (uint16_t(this->buffer_[9]) << 8) | this->buffer_[10], this->buffer_[11]);

  uint8_t uid_length = this->buffer_[12];
  bool report = true;
  uint8_t *uid = &this->buffer_[13];
  for (auto *trigger : this->triggers_) {
    trigger->process(uid, uid_length);
  }
  for (auto *tag : this->binary_sensors_) {
    if (tag->process(uid, uid_length)) {
      report = false;
    }
  }
  bool matches = false;
  if (this->last_uid_.size() == uid_length) {
    matches = true;
    for (size_t i = 0; i < uid_length; i++)
      matches = matches && this->last_uid_[i] == uid[i];
  }

  if (matches) {
    report = false;
  } else {
    this->last_uid_.clear();
    for (size_t i = 0; i < uid_length; i++)
      this->last_uid_.push_back(uid[i]);
  }

  if (report) {
    char buf[32];
    format_uid(buf, uid, uid_length);
    ESP_LOGD(TAG, "Found new tag '%s'", buf);
  }
}

bool PN532BinarySensor::process(uint8_t *data, uint8_t len) {
  if (len != this->uid_.size())
    return false;

  for (uint8_t i = 0; i < len; i++) {
    if (data[i] != this->uid_[i])
      return false;
  }

  this->publish_state(true);
  yield();
  this->publish_state(false);
  return true;
}
float PN532Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void PN532Component::pn532_write_command_(uint8_t len) {
  this->enable();

  // SPI data write
  this->write_byte(0x01);
  // Preamble
  this->write_byte(0x00);
  this->write_byte(0x00);

  // start code 2
  this->write_byte(0xFF);

  // Length, not part of checksum
  const uint8_t real_length = len + 1;
  this->write_byte(real_length);
  this->write_byte(~real_length + 1);

  // Mark that we're sending data, part of checksum
  this->write_byte(0xD4);
  uint8_t checksum = 0xD4;

  for (uint8_t i = 0; i < len; i++) {
    this->write_byte(this->buffer_[i]);
    checksum += this->buffer_[i];
  }

  this->write_byte(~checksum + 1);
  // Postamble
  this->write_byte(0x00);
  this->disable();
}

bool PN532Component::pn532_write_command_check_ack_(uint8_t len, bool ignore) {
  this->pn532_write_command_(len);

  uint32_t start_time = millis();
  while (!this->is_ready()) {
    if (millis() - start_time > 100) {
      if (!ignore) {
        ESP_LOGE(TAG, "Timed out waiting for ACK from PN532!");
      }
      return false;
    }
    yield();
  }

  if (!this->read_ack()) {
    if (!ignore) {
      ESP_LOGE(TAG, "Invalid ACK frame received from PN532!");
    }
    return false;
  }

  return true;
}

void PN532Component::pn532_read_data_(uint8_t len) {
  this->enable();

  // We're reading
  this->write_byte(0x03);

  for (uint8_t i = 0; i < len; i++) {
    this->buffer_[i] = this->read_byte();
  }

  this->disable();
}

PN532BinarySensor *PN532Component::make_tag(const std::string &name, const std::vector<uint8_t> &uid) {
  auto *tag = new PN532BinarySensor(name, uid, this->get_update_interval());
  this->binary_sensors_.push_back(tag);
  return tag;
}

PN532Trigger *PN532Component::make_trigger() {
  auto *trigger = new PN532Trigger();
  this->triggers_.push_back(trigger);
  return trigger;
}
bool PN532Component::is_ready() {
  this->enable();
  // Mark we're reading state
  this->write_byte(0x02);
  bool ret = this->read_byte() & 1;
  this->disable();
  if (ret) {
    ESP_LOGVV(TAG, "Chip is ready!");
  }
  return ret;
}
bool PN532Component::read_ack() {
  ESP_LOGVV(TAG, "Reading ACK...");
  this->enable();
  // Mark we're reading
  this->write_byte(0x03);

  bool matches = true;
  for (uint8_t i : PN532_ACK) {
    uint8_t value = this->read_byte();
    // Reading ACK requires 1ms of delay between bytes for some reason
    delay(1);
    matches = matches && value == i;
  }
  this->disable();
  return matches;
}
PN532Component::PN532Component(SPIComponent *parent, GPIOPin *cs, uint32_t update_interval)
    : PollingComponent(update_interval), SPIDevice(parent, cs) {

}

bool PN532Component::msb_first() {
  return false;
}
void PN532Component::dump_config() {
  ESP_LOGCONFIG(TAG, "PN532:");
  switch (this->error_code_) {
    case NONE:break;
    case WRITING_SAM_COMMAND_FAILED:
      ESP_LOGE(TAG, "Writing SAM command failed!");
      break;
    case READING_SAM_COMMAND_FAILED:
      ESP_LOGE(TAG, "Reading SAM command result failed!");
      break;
  }

  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);

  for (auto *child : this->binary_sensors_) {
    LOG_BINARY_SENSOR("  ", "Tag", child);
  }
}

PN532BinarySensor::PN532BinarySensor(const std::string &name, const std::vector<uint8_t> &uid, uint32_t update_interval)
    : BinarySensor(name), uid_(uid) {
  if (update_interval > 0)
    this->add_filter(App.register_component(new DelayedOffFilter((update_interval * 3) / 2)));
}

void PN532Trigger::process(uint8_t *uid, uint8_t uid_length) {
  char buf[32];
  format_uid(buf, uid, uid_length);
  this->trigger(std::string(buf));
}

} // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif //USE_PN532
