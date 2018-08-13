//
//  sht3xd_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 11.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//
// Based on:
//   - https://cdn-shop.adafruit.com/product-files/2857/Sensirion_Humidity_SHT3x_Datasheet_digital-767294.pdf
//   - https://github.com/Sensirion/arduino-sht

#include "esphomelib/defines.h"

#ifdef USE_SHT3XD

#include "esphomelib/sensor/sht3xd_component.h"
#include "esphomelib/log.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.sht3xd";

static const uint16_t SHT3XD_COMMAND_READ_SERIAL_NUMBER = 0x3780;
static const uint16_t SHT3XD_COMMAND_READ_STATUS = 0xF32D;
static const uint16_t SHT3XD_COMMAND_CLEAR_STATUS = 0x3041;
static const uint16_t SHT3XD_COMMAND_HEATER_ENABLE = 0x306D;
static const uint16_t SHT3XD_COMMAND_HEATER_DISABLE = 0x3066;
static const uint16_t SHT3XD_COMMAND_SOFT_RESET = 0x30A2;
static const uint16_t SHT3XD_COMMAND_POLLING_H = 0x2400;
static const uint16_t SHT3XD_COMMAND_FETCH_DATA = 0xE000;

SHT3XDComponent::SHT3XDComponent(I2CComponent *parent,
                                 const std::string &temperature_name, const std::string &humidity_name,
                                 uint8_t address, uint32_t update_interval)
    : PollingComponent(update_interval), I2CDevice(parent, address),
      temperature_sensor_(new SHT3XDTemperatureSensor(temperature_name, this)),
      humidity_sensor_(new SHT3XDHumiditySensor(humidity_name, this)) {

}

void SHT3XDComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SHT3xD...");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);
  if (!this->write_command(SHT3XD_COMMAND_READ_SERIAL_NUMBER)) {
    ESP_LOGE(TAG, "Communication with SHT3xD failed!");
    this->mark_failed();
    return;
  }

  uint16_t raw_serial_number[2];
  if (!this->read_data(raw_serial_number, 2)) {
    this->mark_failed();
    return;
  }
  uint32_t serial_number = (uint32_t(raw_serial_number[0]) << 16) | uint32_t(raw_serial_number[1]);
  std::string base = uint32_to_string(serial_number);
  ESP_LOGCONFIG(TAG, "    Serial Number: 0x%08X", serial_number);
  this->temperature_sensor_->unique_id_ = base + "-temperature";
  this->humidity_sensor_->unique_id_ = base + "-humidity";
}
float SHT3XDComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void SHT3XDComponent::update() {
  if (!this->write_command(SHT3XD_COMMAND_POLLING_H))
    return;

  this->set_timeout(50, [this](){
    uint16_t raw_data[2];
    if (!this->read_data(raw_data, 2)) {
      this->status_set_warning();
      return;
    }

    float temperature = 175.0f * float(raw_data[0]) / 65535.0f - 45.0f;
    float humidity = 100.0f * float(raw_data[1]) / 65535.0f;

    ESP_LOGD(TAG, "Got temperature=%.2f°C humidity=%.2f%%", temperature, humidity);
    this->temperature_sensor_->push_new_value(temperature);
    this->humidity_sensor_->push_new_value(humidity);
    this->status_clear_warning();
  });
}

bool SHT3XDComponent::write_command(uint16_t command) {
  // Warning ugly, trick the I2Ccomponent base by setting register to the first 8 bit.
  return this->write_byte(command >> 8, command & 0xFF);
}

uint8_t sht_crc(uint8_t data1, uint8_t data2) {
  uint8_t bit;
  uint8_t crc = 0xFF;

  crc ^= data1;
  for (bit = 8; bit > 0; --bit) {
    if (crc & 0x80)
      crc = (crc << 1) ^ 0x131;
    else
      crc = (crc << 1);
  }

  crc ^= data2;
  for (bit = 8; bit > 0; --bit) {
    if (crc & 0x80)
      crc = (crc << 1) ^ 0x131;
    else
      crc = (crc << 1);
  }

  return crc;
}

bool SHT3XDComponent::read_data(uint16_t *data, uint8_t len) {
  const uint8_t num_bytes = len * 3;
  auto *buf = new uint8_t[num_bytes];

  if (!this->parent_->receive_(this->address_, buf, num_bytes)) {
    delete[](buf);
    return false;
  }

  for (uint8_t i = 0; i < len; i++) {
    const uint8_t j = 3 * i;
    uint8_t crc = sht_crc(buf[j], buf[j + 1]);
    if (crc != buf[j + 2]) {
      ESP_LOGE(TAG, "CRC8 Checksum invalid! 0x%02X != 0x%02X", buf[j + 2], crc);
      delete[](buf);
      return false;
    }
    data[i] = (buf[j] << 8) | buf[j + 1];
  }

  delete[](buf);
  return true;
}
SHT3XDTemperatureSensor *SHT3XDComponent::get_temperature_sensor() const {
  return this->temperature_sensor_;
}
SHT3XDHumiditySensor *SHT3XDComponent::get_humidity_sensor() const {
  return this->humidity_sensor_;
}

SHT3XDTemperatureSensor::SHT3XDTemperatureSensor(const std::string &name, SHT3XDComponent *parent)
    : EmptyPollingParentSensor(name, parent) {}
std::string SHT3XDTemperatureSensor::unique_id() {
  return this->unique_id_;
}

SHT3XDHumiditySensor::SHT3XDHumiditySensor(const std::string &name, SHT3XDComponent *parent)
    : EmptyPollingParentSensor(name, parent) {}
std::string SHT3XDHumiditySensor::unique_id() {
  return this->unique_id_;
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SHT3XD
