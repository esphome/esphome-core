#include "esphomelib/defines.h"

#ifdef USE_PZM004T

#include "esphomelib/sensor/pzem004t.h"
#include "esphomelib/log.h"
#include "pzem004t.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.pzem04T";

PZEM004TAddress::PZEM004TAddress(uint8_t address0, uint8_t address1, uint8_t address2, uint8_t address3) {
  this->address_[0] = address0;
  this->address_[1] = address1;
  this->address_[2] = address2;
  this->address_[3] = address3;
}
uint8_t PZEM004TAddress::operator[](int i) const {
  return this->address_[i];
}
void PZEM004TAddress::print_to(char *buffer) {
  sprintf(buffer, "%u.%u.%u.%u", this->address_[0], this->address_[1], this->address_[2], this->address_[3]);
}
void PZEM004TBus::send_(const PZEM004TAddress &addr, PZEM004TActiveMeasurement command, uint8_t data) {
  uint16_t crc = 0;
  uint8_t command_ = static_cast<uint8_t>(command);
  this->write_byte(command_);
  crc += command_;
  this->write_byte(addr[0]);
  crc += addr[0];
  this->write_byte(addr[1]);
  crc += addr[1];
  this->write_byte(addr[2]);
  crc += addr[2];
  this->write_byte(addr[3]);
  crc += addr[3];
  this->write_byte(data);
  crc += data;
  this->active_measurement_ = command;
  this->write_byte(uint8_t(crc & 0xFF));
}
void PZEM004TBus::receive_() {
  while (this->available()) {
    uint8_t data;
    this->read_byte(&data);
    if (this->rx_buffer_at_ == 0 && data == 0) {
      // skip 0 at begin
      continue;
    }

    if (this->rx_buffer_at_ == 1) {
      bool success;
      switch (data) {
        case 0xA0: // VOLTAGE
        case 0xA1: // CURRENT
        case 0xA2: // POWER
        case 0xA3: // ENERGY
        case 0xA4: // SET ADDRESS
        case 0xA5: // POWER ALARM
          success = true;
          break;
        default:
          success = false;
          break;
      }
      if (!success) {
        if (millis() > 10000) {
          // dont emit warning on startup
          ESP_LOGW(TAG, "Invalid start byte: 0x%02X", data);
        }
        this->rx_buffer_at_ = 0;
        continue;
      }
    }

    if (this->rx_buffer_at_ == 6) {
      // checksum
      this->rx_buffer_at_ = 0;

      uint16_t crc = 0;
      for (uint8_t i = 0; i < 6; i++)
        crc += this->rx_buffer_[i];

      if (data != uint8_t(crc & 0xFF)) {
        ESP_LOGW(TAG, "CRC Checksum doesn't match for PZEM004T! (0x%02X != 0x%02X)",
                 data, crc & 0xFF);
        continue;
      }

      this->parse_rx_();
    } else {
      this->rx_buffer_[this->rx_buffer_at_++] = data;
    }
  }
}
void PZEM004TBus::parse_rx_() {
  const uint8_t *data = this->rx_buffer_ + 1;
  PZEM004T pzem = this->pzems_[this->active_index_];

  char address[32];
  pzem.address_.print_to(address);

  switch (this->rx_buffer_[0]) {
    case 0xA0: { // VOLTAGE
      if (this->setup_phase_)
        break;
      float voltage = data[0] * 256.0f + data[1] * 1.0f + data[2] * 0.1f;
      ESP_LOGD(TAG, "%s: Got voltage=%.1fV", address, voltage);
      pzem.send_voltage(voltage);
      this->got_data_ = this->active_measurement_ == PZEM004TActiveMeasurement::VOLTAGE;
      break;
    }
    case 0xA1: { // CURRENT
      if (this->setup_phase_)
        break;
      float current = data[0] * 256.0f + data[1] * 1.0f + data[2] * 0.01f;
      ESP_LOGD(TAG, "%s: Got current=%.1fA", address, current);
      pzem.send_current(current);
      this->got_data_ = this->active_measurement_ == PZEM004TActiveMeasurement::CURRENT;
      break;
    }
    case 0xA2: { // POWER
      if (this->setup_phase_)
        break;
      float power = data[0] * 256.0f + data[1] * 1.0f;
      ESP_LOGD(TAG, "%s: Got power=%.1fW", address, power);
      pzem.send_power(power);
      this->got_data_ = this->active_measurement_ == PZEM004TActiveMeasurement::POWER;
      break;
    }
    case 0xA4: { // SET ADDRESS
      if (!this->setup_phase_)
        break;
      ESP_LOGD(TAG, "Successfully set address to %s!", address);
      this->got_data_ = this->active_measurement_ == PZEM004TActiveMeasurement::SET_ADDRESS;
      break;
    }
    default:
      break;
  }
}
void PZEM004TBus::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PZEM004T...");
  this->setup_phase_ = this->pzems_.size() == 1;
}
float PZEM004TBus::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

static const uint8_t PZEM004T_MEASUREMENTS[3] = {0xB0, 0xB1, };

void PZEM004TBus::update() {
  if (this->setup_phase_) {
    if (this->active_index_ == -1) {
      this->send_(this->pzems_[0].address_, PZEM004TActiveMeasurement, 0);
    } else {
      if (!this->got_data_) {
        ESP_LOGE(TAG, "Setting PZEM004T address failed!");
        this->mark_failed();
        return;
      }
      this->setup_phase_ = false;
    }
  }

  if (this->active_index_ == -1) {
    this->send_(this->pzems_[0].address_, 0xB4, )
  }
}
void PZEM004TBus::dump_config() {

}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_PZM004T
