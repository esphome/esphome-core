//
//  i2c_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/i2c_component.h"
#include "esphomelib/log.h"

#ifdef USE_I2C

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "i2c";

I2CComponent::I2CComponent(uint8_t sda_pin, uint8_t scl_pin, bool scan)
    : sda_pin_(sda_pin), scl_pin_(scl_pin), scan_(scan) {
#ifdef ARDUINO_ARCH_ESP32
  if (next_i2c_bus_num_ == 0)
    wire_ = &Wire;
  else
    wire_ = new TwoWire(next_i2c_bus_num_);
  next_i2c_bus_num_++;
#else
  wire_ = &Wire;
#endif
}

void I2CComponent::set_sda_pin(uint8_t sda_pin) {
  this->sda_pin_ = sda_pin;
}
void I2CComponent::set_scl_pin(uint8_t scl_pin) {
  this->scl_pin_ = scl_pin;
}
void I2CComponent::set_scan(bool scan) {
  this->scan_ = scan;
}
void I2CComponent::set_frequency(uint32_t frequency) {
  this->frequency_ = frequency;
}

void I2CComponent::setup() {
  this->wire_->begin(this->sda_pin_, this->scl_pin_);
  this->wire_->setClock(this->frequency_);
}
void I2CComponent::loop() {
  if (this->scan_) {
    this->scan_ = false;
    ESP_LOGI(TAG, "Scanning i2c bus for active devices...");
    for (uint8_t address = 8; address < 120; address++) {
      this->wire_->beginTransmission(address);
      uint8_t error = this->wire_->endTransmission();

      if (error == 0) {
        ESP_LOGI(TAG, "Found i2c device at address 0x%02X", address);
      } else if (error == 4) {
        ESP_LOGI(TAG, "Unknown error at address 0x%02X", address);
      }

      delay(1);
    }
  }
}
float I2CComponent::get_setup_priority() const {
  return setup_priority::HARDWARE + 10.0f;
}
void I2CComponent::set_receive_timeout(uint32_t receive_timeout) {
  this->receive_timeout_ = receive_timeout;
}



void I2CComponent::begin_transmission_(uint8_t address) {
  ESP_LOGVV(TAG, "Beginning Transmission to 0x%02X:", address);
  this->wire_->beginTransmission(address);
}
bool I2CComponent::end_transmission_(uint8_t address) {
  uint8_t status = this->wire_->endTransmission();
  ESP_LOGVV(TAG, "    Transmission ended. Status code: 0x%02X", status);

  switch (status) {
    case 0:
      break;
    case 1:
      ESP_LOGW(TAG, "Too much data to fit in transmitter buffer for address 0x%02X", address);
      break;
    case 2:
      ESP_LOGW(TAG, "Received NACK on transmit of address 0x%02X", address);
      break;
    case 3:
      ESP_LOGW(TAG, "Received NACK on transmit of data for address 0x%02X", address);
      break;
    default:
      ESP_LOGW(TAG, "Unknown transmit error %u for address 0x%02X", status, address);
      break;
  }

  return status == 0;
}
void I2CComponent::request_from_(uint8_t address, uint8_t len) {
  ESP_LOGVV(TAG, "Requesting %u bytes from 0x%02X:", len, address);
  this->wire_->requestFrom(address, len);
}
void I2CComponent::write_(uint8_t address, const uint8_t *data, uint8_t len) {
  for (size_t i = 0; i < len; i++) {
    ESP_LOGVV(TAG, "    Writing 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)",
              BYTE_TO_BINARY(data[i]), data[i]);
    this->wire_->write(data[i]);
  }
}
void I2CComponent::write_16_(uint8_t address, const uint16_t *data, uint8_t len) {
  for (size_t i = 0; i < len; i++) {
    ESP_LOGVV(TAG, "    Writing 0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN " (0x%04X)",
              BYTE_TO_BINARY(data[i] >> 8), BYTE_TO_BINARY(data[i]), data[i]);
    this->wire_->write(data[i] >> 8);
    this->wire_->write(data[i]);
  }
}
bool I2CComponent::read_(uint8_t address, uint8_t *data) {
  uint32_t start = millis();
  while (this->wire_->available() == 0) {
    if (millis() - start > this->receive_timeout_) {
      ESP_LOGE(TAG, "Receive timeout for address 0x%02X", address);
      return false;
    }
    yield();
  }

  *data = this->wire_->read();
  ESP_LOGVV(TAG, "    Received 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)",
            BYTE_TO_BINARY(*data), *data);
  return true;
}


bool I2CComponent::receive_(uint8_t address, uint8_t *data, uint8_t len) {
  this->request_from_(address, len);
  for (uint8_t i = 0; i < len; i++) {
    if (!this->read_(address, data + i))
      return false;
  }
  return true;
}
bool I2CComponent::receive_16_(uint8_t address, uint16_t *data, uint8_t len) {
  this->request_from_(address, len * 2);
  auto *data_8 = reinterpret_cast<uint8_t *>(data);
  for (uint8_t i = 0; i < len; i++) {
    if (!this->read_(address, data_8 + i * 2 + 1))
      return false;
    if (!this->read_(address, data_8 + i * 2))
      return false;
  }
  return true;
}
bool I2CComponent::read_bytes(uint8_t address, uint8_t register_, uint8_t *data, uint8_t len, uint32_t conversion) {
  if (!this->write_bytes(address, register_, nullptr, 0))
    return false;

  if (conversion > 0)
    delay(conversion);
  return this->receive_(address, data, len);
}
bool I2CComponent::read_bytes_16(uint8_t address, uint8_t register_, uint16_t *data, uint8_t len, uint32_t conversion) {
  if (!this->write_bytes(address, register_, nullptr, 0))
    return false;

  if (conversion > 0)
    delay(conversion);
  return this->receive_16_(address, data, len);
}
bool I2CComponent::read_byte(uint8_t address, uint8_t register_, uint8_t *data, uint32_t conversion) {
  return this->read_bytes(address, register_, data, 1, conversion);
}
bool I2CComponent::read_byte_16(uint8_t address, uint8_t register_, uint16_t *data, uint32_t conversion) {
  return this->read_bytes_16(address, register_, data, 1, conversion);
}
bool I2CComponent::write_bytes(uint8_t address, uint8_t register_, const uint8_t *data, uint8_t len) {
  this->begin_transmission_(address);
  this->write_(address, &register_, 1);
  this->write_(address, data, len);
  return this->end_transmission_(address);
}
bool I2CComponent::write_bytes_16(uint8_t address, uint8_t register_, const uint16_t *data, uint8_t len) {

  this->begin_transmission_(address);
  this->write_(address, &register_, 1);
  this->write_16_(address, data, len);
  return this->end_transmission_(address);
}
bool I2CComponent::write_byte(uint8_t address, uint8_t register_, uint8_t data) {
  return this->write_bytes(address, register_, &data, 1);
}
bool I2CComponent::write_byte_16(uint8_t address, uint8_t register_, uint16_t data) {
  return this->write_bytes_16(address, register_, &data, 0);
}

I2CDevice::I2CDevice(I2CComponent *parent, uint8_t address) : address_(address), parent_(parent) {
  assert(this->parent_ != nullptr && "You have to setup i2c first!");
}

void I2CDevice::set_address(uint8_t address) {
  this->address_ = address;
}
bool I2CDevice::read_bytes(uint8_t register_, uint8_t *data, uint8_t len, uint32_t conversion) {
  return this->parent_->read_bytes(this->address_, register_, data, len, conversion);
}
bool I2CDevice::read_byte(uint8_t register_, uint8_t *data, uint32_t conversion) {
  return this->parent_->read_byte(this->address_, register_, data, conversion);
}
bool I2CDevice::write_bytes(uint8_t register_, const uint8_t *data, uint8_t len) {
  return this->parent_->write_bytes(this->address_, register_, data, len);
}
bool I2CDevice::write_byte(uint8_t register_, uint8_t data) {
  return this->parent_->write_byte(this->address_, register_, data);
}
bool I2CDevice::read_bytes_16(uint8_t register_, uint16_t *data, uint8_t len, uint32_t conversion) {
  return this->parent_->read_bytes_16(this->address_, register_, data, len, conversion);
}
bool I2CDevice::read_byte_16(uint8_t register_, uint16_t *data, uint32_t conversion) {
  return this->parent_->read_byte_16(this->address_, register_, data, conversion);
}
bool I2CDevice::write_bytes_16(uint8_t register_, const uint16_t *data, uint8_t len) {
  return this->parent_->write_bytes_16(this->address_, register_, data, len);
}
bool I2CDevice::write_byte_16(uint8_t register_, uint16_t data) {
  return this->parent_->write_byte_16(this->address_, register_, data);
}
void I2CDevice::set_parent(I2CComponent *parent) {
  this->parent_ = parent;
}

#ifdef ARDUINO_ARCH_ESP32
  uint8_t next_i2c_bus_num_ = 0;
#endif

ESPHOMELIB_NAMESPACE_END

#endif //USE_I2C
