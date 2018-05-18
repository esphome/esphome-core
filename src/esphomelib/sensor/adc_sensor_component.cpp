//
//  adc_sensor_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 24.02.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/adc_sensor_component.h"

#include "esphomelib/log.h"

#ifdef USE_ADC_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.adc";

ADCSensorComponent::ADCSensorComponent(const std::string &name, GPIOInputPin pin, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), pin_(pin) { }

#ifdef ARDUINO_ARCH_ESP32
adc_attenuation_t ADCSensorComponent::get_attenuation() const {
  return this->attenuation_;
}
void ADCSensorComponent::set_attenuation(adc_attenuation_t attenuation) {
  this->attenuation_ = attenuation;
}
#endif

void ADCSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADC '%s'...", this->get_name().c_str());
  this->pin_.setup();

#ifdef ARDUINO_ARCH_ESP32
  analogSetPinAttenuation(this->pin_.get_pin(), this->attenuation_);
#endif
}
float ADCSensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
GPIOInputPin &ADCSensorComponent::get_pin() {
  return this->pin_;
}
void ADCSensorComponent::set_pin(const GPIOInputPin &pin) {
  this->pin_ = pin;
}
void ADCSensorComponent::update() {
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
  this->push_new_value(value_v);
}
std::string ADCSensorComponent::unit_of_measurement() {
  return "V";
}
std::string ADCSensorComponent::icon() {
  return "mdi:flash";
}
int8_t ADCSensorComponent::accuracy_decimals() {
  return 2;
}
#ifdef ARDUINO_ARCH_ESP8266
std::string ADCSensorComponent::unique_id() {
  return get_mac_address() + "-adc";
}
#endif

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_ADC_SENSOR
