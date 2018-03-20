//
//  adc_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 24.02.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/input/adc_component.h"

#include "esphomelib/log.h"

namespace esphomelib {

namespace input {

static const char *TAG = "input::adc";

#ifdef ARDUINO_ARCH_ESP32
ADCSensorComponent::ADCSensorComponent(uint8_t pin, uint32_t update_interval, uint8_t mode)
    : Sensor(update_interval), mode_(mode) {
  this->set_pin(pin);
}
#else
ADCSensorComponent::ADCSensorComponent(uint8_t pin, uint32_t update_interval)
    :  Sensor(update_interval) {
  this->set_pin(pin);
}
#endif

uint8_t ADCSensorComponent::get_pin() const {
  return this->pin_;
}
void ADCSensorComponent::set_pin(uint8_t pin) {
  assert_construction_state(this);
#ifdef ARDUINO_ARCH_ESP32
  assert(32 <= pin && pin <= 39 && "Only pins 32 through 39 support ADC.");
#else
  assert(pin == A0 && "Only pin A0 supports ADC.");
#endif
  this->pin_ = pin;
}

#ifdef ARDUINO_ARCH_ESP32
adc_attenuation_t ADCSensorComponent::get_attenuation() const {
  return this->attenuation_;
}
void ADCSensorComponent::set_attenuation(adc_attenuation_t attenuation) {
  assert_construction_state(this);
  this->attenuation_ = attenuation;
}
uint8_t ADCSensorComponent::get_mode() const {
  return this->mode_;
}
void ADCSensorComponent::set_mode(uint8_t mode) {
  assert_construction_state(this);
  this->mode_ = mode;
}
#endif

void ADCSensorComponent::setup() {
#ifdef ARDUINO_ARCH_ESP32
  pinMode(this->pin_, this->mode_);

  analogSetPinAttenuation(this->pin_, this->attenuation_);
#endif

  this->set_interval("retrieve_adc", this->get_update_interval(), [&]() {
    uint16_t value = analogRead(this->pin_);
    float value_v = value / 4095.0f;
#ifdef ARDUINO_ARCH_ESP32
    switch (this->attenuation_) {
      case ADC_0db:
        value_v *= 1.1;
        break;
      case ADC_2_5db:
        value_v *= 1.5;
        break;
      case ADC_6db:
        value_v *= 2.2;
        break;
      case ADC_11db:
        value_v *= 3.9;
        break;
    }
#endif
    this->push_new_value(value_v, 2);
  });
}
float ADCSensorComponent::get_setup_priority() const {
  return Component::get_setup_priority();
}
std::string ADCSensorComponent::unit_of_measurement() {
  return "V";
}

} // namespace input

} // namespace esphomelib
