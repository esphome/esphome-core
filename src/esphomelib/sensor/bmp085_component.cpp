//
//  bmp085_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 27.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//
// Based on:
//   - https://www.sparkfun.com/datasheets/Components/General/BST-BMP085-DS000-05.pdf
//   - https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/BMP085

#include "esphomelib/defines.h"

#ifdef USE_BMP085_SENSOR

#include "esphomelib/sensor/bmp085_component.h"

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.bmp085";

static const uint8_t BMP085_ADDRESS = 0x77;
static const uint8_t BMP085_REGISTER_AC1_H = 0xAA;
static const uint8_t BMP085_REGISTER_CONTROL = 0xF4;
static const uint8_t BMP085_REGISTER_DATA_MSB  = 0xF6;
static const uint8_t BMP085_CONTROL_MODE_TEMPERATURE = 0x2E;
static const uint8_t BMP085_CONTROL_MODE_PRESSURE_3 = 0xF4;

void BMP085Component::update() {
  if (!this->set_mode_(BMP085_CONTROL_MODE_TEMPERATURE))
    return;

  this->set_timeout("temperature", 5, [this]() { this->read_temperature_(); });
}
void BMP085Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up BMP085...");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);
  uint8_t data[22];
  if (!this->read_bytes(BMP085_REGISTER_AC1_H, data, 22)) {
    ESP_LOGE(TAG, "Connection to BMP085 failed.");
    this->mark_failed();
    return;
  }

  // Load calibration
  this->calibration_.ac1 = ((data[0] & 0xFF) << 8) | (data[1] & 0xFF);
  this->calibration_.ac2 = ((data[2] & 0xFF) << 8) | (data[3] & 0xFF);
  this->calibration_.ac3 = ((data[4] & 0xFF) << 8) | (data[5] & 0xFF);
  this->calibration_.ac4 = ((data[6] & 0xFF) << 8) | (data[7] & 0xFF);
  this->calibration_.ac5 = ((data[8] & 0xFF) << 8) | (data[9] & 0xFF);
  this->calibration_.ac6 = ((data[10] & 0xFF) << 8) | (data[11] & 0xFF);
  this->calibration_.b1 = ((data[12] & 0xFF) << 8) | (data[13] & 0xFF);
  this->calibration_.b2 = ((data[14] & 0xFF) << 8) | (data[15] & 0xFF);
  this->calibration_.mb = ((data[16] & 0xFF) << 8) | (data[17] & 0xFF);
  this->calibration_.mc = ((data[18] & 0xFF) << 8) | (data[19] & 0xFF);
  this->calibration_.md = ((data[20] & 0xFF) << 8) | (data[21] & 0xFF);
}
BMP085Component::BMP085Component(I2CComponent *parent,
                                 const std::string &temperature_name, const std::string &pressure_name,
                                 uint32_t update_interval)
    : PollingComponent(update_interval), I2CDevice(parent, BMP085_ADDRESS),
      temperature_(new BMP085TemperatureSensor(temperature_name, this)),
      pressure_(new BMP085PressureSensor(pressure_name, this)) {

}

void BMP085Component::read_temperature_() {
  uint8_t buffer[2];
  if (!this->read_bytes(BMP085_REGISTER_DATA_MSB, buffer, 2)) {
    this->status_set_warning();
    return;
  }

  int32_t ut = ((buffer[0] & 0xFF) << 8) | (buffer[1] & 0xFF);
  if (ut == 0) {
    ESP_LOGW(TAG, "Invalid temperature!");
    this->status_set_warning();
    return;
  }

  int32_t x1 = ((ut - int32_t(this->calibration_.ac6)) * int32_t(this->calibration_.ac5)) >>15;
  int32_t x2 = int32_t(this->calibration_.mc << 11) / (x1 + this->calibration_.md);
  this->calibration_.b5 =  x1 + x2;
  float temperature = (this->calibration_.b5 >> 4) / 10.0f;
  ESP_LOGD(TAG, "Got Temperature=%.1f°C", temperature);
  this->temperature_->push_new_value(temperature);
  this->status_clear_warning();

  if (!this->set_mode_(BMP085_CONTROL_MODE_PRESSURE_3)) {
    this->status_set_warning();
    return;
  }

  this->set_timeout("pressure", 26, [this]() { this->read_pressure_(); });
}
void BMP085Component::read_pressure_() {
  uint8_t buffer[3];
  if (!this->read_bytes(BMP085_REGISTER_DATA_MSB, buffer, 3)) {
    this->status_set_warning();
    return;
  }

  uint32_t value = (uint32_t(buffer[0]) << 16) | (uint32_t(buffer[1]) << 8) | uint32_t(buffer[0]);
  value = value >> 5;
  if (value == 0) {
    this->status_set_warning();
    ESP_LOGW(TAG, "Invalid pressure!");
    return;
  }

  uint8_t oss = 3;
  int32_t p;
  int32_t b6 = this->calibration_.b5 - 4000;
  int32_t x1 = (int32_t(this->calibration_.b2) * ((b6 * b6) >> 12)) >> 11;
  int32_t x2 = (int32_t(this->calibration_.ac2) * b6) >> 11;
  int32_t x3 = x1 + x2;
  int32_t b3 = (((int32_t(this->calibration_.ac1) * 4 + x3) << oss) + 2) >> 2;
  x1 = (int32_t(this->calibration_.ac3) * b6) >> 13;
  x2 = (int32_t(this->calibration_.b1) * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  uint32_t b4 = (uint32_t(this->calibration_.ac4) * uint32_t(x3 + 32768)) >> 15;
  uint32_t b7 = ((uint32_t) value - b3) * (uint32_t) (50000UL >> oss);
  if (b7 < 0x80000000) p = (b7 << 1) / b4;
  else p = (b7 / b4) << 1;

  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  float pressure = (p + ((x1 + x2 + 3791) >> 4)) / 100.0f;

  ESP_LOGD(TAG, "Got Pressure=%.1fhPa", pressure);

  this->pressure_->push_new_value(pressure);
  this->status_clear_warning();
}
BMP085TemperatureSensor *BMP085Component::get_temperature_sensor() const {
  return this->temperature_;
}
BMP085PressureSensor *BMP085Component::get_pressure_sensor() const {
  return this->pressure_;
}
bool BMP085Component::set_mode_(uint8_t mode) {
  ESP_LOGV(TAG, "Setting mode to 0x%02X...", mode);
  return this->write_byte(BMP085_REGISTER_CONTROL, mode);
}
float BMP085Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BMP085_SENSOR
