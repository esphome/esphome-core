//
//  pcf8574_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/io/pcf8574_component.h"
#include "esphomelib/log.h"

#ifdef USE_PCF8574

#include <Wire.h>

namespace esphomelib {

namespace io {

static const char *TAG = "io.pcf8574";

PCF8574Component::PCF8574Component(uint8_t address, bool pcf8575)
    : address_(address), pcf8575_(pcf8575) {}
void PCF8574Component::setup() {
  if (!this->read_gpio_()) {
    ESP_LOGE(TAG, "PCF8574 not available under %x", this->address_);
    this->mark_failed();
    return;
  }

  this->write_gpio_();
  this->read_gpio_();
}
void PCF8574Component::loop() {
  this->write_gpio_();
  this->read_gpio_();
}
bool PCF8574Component::digital_read_(uint8_t pin) {
  return this->input_mask_ & (1 << pin);
}
void PCF8574Component::digital_write_(uint8_t pin, bool value) {
  if (value) {
    this->port_mask_ |= (1 << pin);
  } else {
    this->port_mask_ &= ~(1 << pin);
  }
}
void PCF8574Component::pin_mode_(uint8_t pin, uint8_t mode) {
  switch (mode) {
    case PCF8574_INPUT:
      this->ddr_mask_ &= ~(1 << pin);
      this->port_mask_ &= ~(1 << pin);
      break;
    case PCF8574_INPUT_PULLUP:
      this->ddr_mask_ &= ~(1 << pin);
      this->port_mask_ |= (1 << pin);
      break;
    case PCF8574_OUTPUT:
      this->ddr_mask_ |= (1 << pin);
      this->port_mask_ &= ~(1 << pin);
      break;
    default:
      assert(false);
  }
}
bool PCF8574Component::read_gpio_() {
  uint8_t quantity = this->pcf8575_ ? 2 : 1;
  if (Wire.requestFrom(this->address_, quantity) != 1) {
    ESP_LOGW(TAG, "Communicating with PCF8574 failed.");
    return false;
  }

  while (Wire.available() < 2);

  this->input_mask_ = Wire.read() & 0xFF; // LSB first

  if (this->pcf8575_) {
    this->input_mask_ |= (Wire.read() & 0xFF) << 8;
  }

  return true;
}
void PCF8574Component::write_gpio_() {
  Wire.beginTransmission(this->address_);

  uint16_t value = (this->input_mask_ & ~this->ddr_mask_) | this->port_mask_;

  Wire.write(value & 0xFF);
  if (this->pcf8575_) {
    Wire.write((value & 0xFF00) >> 8);
  }

  Wire.endTransmission();
}
PCF8574GPIOInputPin PCF8574Component::make_input_pin(uint8_t pin, uint8_t mode, bool inverted) {
  assert(mode <= PCF8574_OUTPUT);
  if (this->pcf8575_) { assert(pin < 16); }
  else { assert(pin < 8); }
  return {this, pin, mode, inverted};
}
PCF8574GPIOOutputPin PCF8574Component::make_output_pin(uint8_t pin, bool inverted) {
  if (this->pcf8575_) { assert(pin < 16); }
  else { assert(pin < 8); }
  return {this, pin, PCF8574_OUTPUT, inverted};
}

void PCF8574GPIOInputPin::setup() const {
  this->parent_->pin_mode_(this->pin_, this->mode_);
}
bool PCF8574GPIOInputPin::digital_read() const {
  return this->parent_->digital_read_(this->pin_) != this->inverted_;
}
void PCF8574GPIOInputPin::digital_write(bool value) const {
  this->parent_->digital_write_(this->pin_, value != this->inverted_);
}
PCF8574GPIOInputPin::PCF8574GPIOInputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOInputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *PCF8574GPIOInputPin::copy() const {
  return new PCF8574GPIOInputPin(*this);
}

void PCF8574GPIOOutputPin::setup() const {
  this->parent_->pin_mode_(this->pin_, this->mode_);
}
bool PCF8574GPIOOutputPin::digital_read() const {
  return this->parent_->digital_read_(this->pin_) != this->inverted_;
}
void PCF8574GPIOOutputPin::digital_write(bool value) const {
  this->parent_->digital_write_(this->pin_, value != this->inverted_);
}
PCF8574GPIOOutputPin::PCF8574GPIOOutputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOOutputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *PCF8574GPIOOutputPin::copy() const {
  return new PCF8574GPIOOutputPin(*this);
}

} // namespace io

} // namespace esphomelib

#endif //USE_PCF8574
