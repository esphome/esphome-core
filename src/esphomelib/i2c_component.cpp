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

#include <Wire.h>

namespace esphomelib {

static const char *TAG = "i2c";

I2CComponent::I2CComponent(uint8_t sda_pin, uint8_t scl_pin, bool scan)
    : sda_pin_(sda_pin), scl_pin_(scl_pin), scan_(scan) {}

uint8_t I2CComponent::get_sda_pin() const {
  return this->sda_pin_;
}
void I2CComponent::set_sda_pin(uint8_t sda_pin) {
  this->sda_pin_ = sda_pin;
}
uint8_t I2CComponent::get_scl_pin() const {
  return this->scl_pin_;
}
void I2CComponent::set_scl_pin(uint8_t scl_pin) {
  this->scl_pin_ = scl_pin;
}
void I2CComponent::set_scan(bool scan) {
  this->scan_ = scan;
}

#ifdef ARDUINO_ARCH_ESP32
float I2CComponent::get_frequency() const {
  return this->frequency_;
}
void I2CComponent::set_frequency(float frequency) {
  this->frequency_ = frequency;
}
#endif

void I2CComponent::setup() {
#ifdef ARDUINO_ARCH_ESP32
  Wire.begin(this->sda_pin_, this->scl_pin_, this->frequency_);
#else
  Wire.begin(this->sda_pin_, this->scl_pin_);
#endif
}
void I2CComponent::loop() {
  if (this->scan_) {
    this->scan_ = false;
    ESP_LOGI(TAG, "Scanning i2c bus for active devices...");
    for (uint8_t address = 8; address < 120; address++) {
      Wire.beginTransmission(address);
      uint8_t error = Wire.endTransmission();

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

} // namespace esphomelib

#endif //USE_I2C
