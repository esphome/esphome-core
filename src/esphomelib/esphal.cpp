//
//  esphal.cpp
//  esphomelib
//
//  Created by Otto Winter on 21.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/esphal.h"

namespace esphomelib {

GPIOPin::GPIOPin(uint8_t pin, uint8_t mode) {
  this->set_pin(pin);
  this->set_mode(mode);
}

unsigned char GPIOPin::get_pin() const {
  return this->pin_;
}

void GPIOPin::set_pin(unsigned char pin) {
  this->pin_ = pin;
}

unsigned char GPIOPin::get_mode() const {
  return this->mode_;
}

void GPIOPin::set_mode(unsigned char mode) {
  this->mode_ = mode;
}

bool GPIOPin::read_value() const {
  return (digitalRead(this->pin_) == HIGH) != this->inverted_;
}

void GPIOPin::write_value(bool value) const {
  digitalWrite(this->pin_, uint8_t(value != this->inverted_));
}

bool GPIOPin::is_inverted() const {
  return this->inverted_;
}

void GPIOPin::set_inverted(bool inverted) {
  this->inverted_ = inverted;
}
void GPIOPin::setup() {
  pinMode(this->pin_, this->mode_);
}

GPIOOutputPin::GPIOOutputPin(uint8_t pin, uint8_t mode)
    : GPIOPin(pin, mode) {}

GPIOInputPin::GPIOInputPin(uint8_t pin, uint8_t mode)
    : GPIOPin(pin, mode) {}

} // namespace esphomelib
