//
//  adc_component.h
//  esphomelib
//
//  Created by Otto Winter on 24.02.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_INPUT_ADC_COMPONENT_H
#define ESPHOMELIB_INPUT_ADC_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

namespace input {

class ADCSensorComponent : public Component, public sensor::Sensor {
 public:
#ifdef ARDUINO_ARCH_ESP32
  explicit ADCSensorComponent(uint8_t pin, uint32_t update_interval = 15000, uint8_t mode = ANALOG);
#else
  explicit ADCSensorComponent(uint8_t pin, uint32_t update_interval = 15000);
#endif

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);

#ifdef ARDUINO_ARCH_ESP32
  uint8_t get_mode() const;
  void set_mode(uint8_t mode);
#endif

  void setup() override;
  float get_setup_priority() const override;
  std::string unit_of_measurement() override;

#ifdef ARDUINO_ARCH_ESP32
  adc_attenuation_t get_attenuation() const;
  void set_attenuation(adc_attenuation_t attenuation);
#endif

 protected:
  uint8_t pin_;

#ifdef ARDUINO_ARCH_ESP32
  uint8_t mode_;
  adc_attenuation_t attenuation_{ADC_0db};
#endif
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_ADC_COMPONENT_H
