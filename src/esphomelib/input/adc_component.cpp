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

ADCSensorComponent::ADCSensorComponent(GPIOInputPin pin, uint32_t update_interval)
    : VoltageSensor(update_interval), pin_(pin) { }

#ifdef ARDUINO_ARCH_ESP32
adc_attenuation_t ADCSensorComponent::get_attenuation() const {
  return this->attenuation_;
}
void ADCSensorComponent::set_attenuation(adc_attenuation_t attenuation) {
  assert_construction_state(this);
  this->attenuation_ = attenuation;
}
#endif

void ADCSensorComponent::setup() {
  this->pin_.setup();

#ifdef ARDUINO_ARCH_ESP32
  analogSetPinAttenuation(this->pin_.get_pin(), this->attenuation_);
#endif

  this->set_interval("retrieve_adc", this->get_update_interval(), [&]() {
    uint16_t value = analogRead(this->pin_.get_pin());
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
GPIOInputPin &ADCSensorComponent::get_pin() {
  return this->pin_;
}
void ADCSensorComponent::set_pin(const GPIOInputPin &pin) {
  this->pin_ = pin;
}

} // namespace input

} // namespace esphomelib
