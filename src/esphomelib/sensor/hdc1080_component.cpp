//
//  hdc1080_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 01.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/hdc1080_component.h"

#include <Wire.h>

#include "esphomelib/log.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor.hdc1080";
static const uint8_t HDC1080_ADDRESS = 0x40; // 0b1000000 from datasheet
static const uint8_t HDC1080_CMD_CONFIGURATION = 0x02;
static const uint8_t HDC1080_CMD_TEMPERATURE = 0x00;
static const uint8_t HDC1080_CMD_HUMIDITY = 0x01;

HDC1080Component::HDC1080Component(uint32_t update_interval)
    : PollingComponent(update_interval),
      temperature_(new HDC1080TemperatureSensor(this)), humidity_(new HDC1080HumiditySensor(this)) {

}
void HDC1080Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HDC1080...");

  Wire.beginTransmission(HDC1080_ADDRESS);
  Wire.write(HDC1080_CMD_CONFIGURATION);
  Wire.write(0b00000000); // resolution 14bit for both humidity and temperature
  Wire.write(0x00); // reserved
  uint8_t result = Wire.endTransmission();

  if (result != 0) {
    ESP_LOGE(TAG, "Connection to HDC1080 failed: %u", result);
    this->mark_failed();
    return;
  }
}
void HDC1080Component::update() {
  uint16_t raw_temp = this->read_data_(HDC1080_CMD_TEMPERATURE);
  float temp = raw_temp * 0.0025177f - 40.0f; // raw * 2^-16 * 165 - 40
  this->temperature_->push_new_value(temp);

  uint16_t raw_humidity = this->read_data_(HDC1080_CMD_HUMIDITY);
  float humidity = raw_humidity * 0.001525879f; // raw * 2^-16 * 100
  this->humidity_->push_new_value(humidity);
}
uint16_t HDC1080Component::read_data_(uint8_t cmd) {
  Wire.beginTransmission(HDC1080_ADDRESS);
  Wire.write(cmd);
  Wire.endTransmission();
  delay(9); // 9ms shouldn't be too bad. Maybe convert this to async code some time.
  Wire.requestFrom(HDC1080_ADDRESS, 2u); // read 2 bytes
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  return uint16_t(msb) << 8 | lsb;
}
HDC1080TemperatureSensor *HDC1080Component::get_temperature_sensor() const {
  return this->temperature_;
}
HDC1080HumiditySensor *HDC1080Component::get_humidity_sensor() const {
  return this->humidity_;
}

HDC1080HumiditySensor::HDC1080HumiditySensor(HDC1080Component *parent) : parent_(parent) {}
std::string HDC1080HumiditySensor::unit_of_measurement() {
  return "%";
}
std::string HDC1080HumiditySensor::icon() {
  return "mdi:water-percent";
}
uint32_t HDC1080HumiditySensor::update_interval() {
  return this->parent_->get_update_interval();
}
int8_t HDC1080HumiditySensor::accuracy_decimals() {
  return 1;
}
HDC1080TemperatureSensor::HDC1080TemperatureSensor(HDC1080Component *parent) : parent_(parent) {}
std::string HDC1080TemperatureSensor::unit_of_measurement() {
  return "°C";
}
std::string HDC1080TemperatureSensor::icon() {
  return "";
}
uint32_t HDC1080TemperatureSensor::update_interval() {
  return this->parent_->get_update_interval();
}
int8_t HDC1080TemperatureSensor::accuracy_decimals() {
  return 1;
}

} // namespace sensor

} // namespace esphomelib
