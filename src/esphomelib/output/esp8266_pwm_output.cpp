//
//  esp8266_pwm_output.cpp
//  esphomelib
//
//  Created by Otto Winter on 25.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifdef ARDUINO_ARCH_ESP8266

#include "esphomelib/output/esp8266_pwm_output.h"

#include "esphomelib/log.h"

namespace esphomelib {

namespace output {

GPIOOutputPin &ESP8266PWMOutput::get_pin() {
  return this->pin_;
}
void ESP8266PWMOutput::set_pin(const GPIOOutputPin &pin) {
  this->pin_ = pin;
}
ESP8266PWMOutput::ESP8266PWMOutput(const GPIOOutputPin &pin)
    : pin_(pin) {}
void ESP8266PWMOutput::setup() {
  assert(this->pin_.get_pin() <= 16);
  this->pin_.setup();
  this->set_state_(0.0f);
}
void ESP8266PWMOutput::write_state(float state) {
  const uint16_t max_duty = 1023;
  auto duty = static_cast<uint16_t>(state * max_duty);
  analogWrite(this->pin_.get_pin(), duty);
}
float ESP8266PWMOutput::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

} // namespace output

} // namespace esphomelib

#endif
