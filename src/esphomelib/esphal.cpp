//
//  esphal.cpp
//  esphomelib
//
//  Created by Otto Winter on 21.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/esphal.h"
#include "esphomelib/log.h"

namespace esphomelib {

static const char *TAG = "esphal";

GPIOPin::GPIOPin(uint8_t pin, uint8_t mode, bool inverted) {
  this->set_pin(pin);
  this->set_mode(mode);
  this->set_inverted(inverted);
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
  if_config {
    std::string mode_s;
    switch (this->mode_) {
      case INPUT: mode_s = "INPUT"; break;
      case OUTPUT: mode_s = "OUTPUT"; break;
      case INPUT_PULLUP: mode_s = "INPUT_PULLUP"; break;
      case OUTPUT_OPEN_DRAIN: mode_s = "OUTPUT_OPEN_DRAIN"; break;
      case SPECIAL: mode_s = "SPECIAL"; break;
      case FUNCTION_1: mode_s = "FUNCTION_1"; break;
      case FUNCTION_2: mode_s = "FUNCTION_2"; break;
      case FUNCTION_3: mode_s = "FUNCTION_3"; break;
      case FUNCTION_4: mode_s = "FUNCTION_4"; break;

#ifdef ARDUINO_ARCH_ESP32
      case PULLUP: mode_s = "PULLUP"; break;
      case PULLDOWN: mode_s = "PULLDOWN"; break;
      case INPUT_PULLDOWN: mode_s = "INPUT_PULLDOWN"; break;
      case OPEN_DRAIN: mode_s = "OPEN_DRAIN"; break;
      case FUNCTION_5: mode_s = "FUNCTION_5"; break;
      case FUNCTION_6: mode_s = "FUNCTION_6"; break;
      case ANALOG: mode_s = "ANALOG"; break;
#endif
#ifdef ARDUINO_ARCH_ESP8266
      case FUNCTION_0: mode_s = "FUNCTION_0"; break;
      case WAKEUP_PULLUP: mode_s = "WAKEUP_PULLUP"; break;
      case WAKEUP_PULLDOWN: mode_s = "WAKEUP_PULLDOWN"; break;
      case INPUT_PULLDOWN_16: mode_s = "INPUT_PULLDOWN_16"; break;
#endif

      default: mode_s = "UNKNOWN"; break;
    }
    ESP_LOGCONFIG(TAG, "    GPIO Pin %u with mode %s", this->pin_, mode_s.c_str());
  }
  pinMode(this->pin_, this->mode_);

}
GPIOPin::GPIOPin() : pin_(0), inverted_(false), mode_(0) {

}

GPIOOutputPin::GPIOOutputPin(uint8_t pin, uint8_t mode, bool inverted)
    : GPIOPin(pin, mode, inverted) {}
GPIOOutputPin::GPIOOutputPin() : GPIOPin() {

}

GPIOInputPin::GPIOInputPin(uint8_t pin, uint8_t mode, bool inverted)
    : GPIOPin(pin, mode, inverted) {}
GPIOInputPin::GPIOInputPin() : GPIOPin() {

}

} // namespace esphomelib
