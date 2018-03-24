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

class ADCSensorComponent : public sensor::PollingSensorComponent {
 public:
  explicit ADCSensorComponent(GPIOInputPin pin, uint32_t update_interval = 15000);

  GPIOInputPin &get_pin();
  void set_pin(const GPIOInputPin &pin);

  void update() override;

  void setup() override;
  float get_setup_priority() const override;

#ifdef ARDUINO_ARCH_ESP32
  adc_attenuation_t get_attenuation() const;
  void set_attenuation(adc_attenuation_t attenuation);
#endif
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  GPIOInputPin pin_;

#ifdef ARDUINO_ARCH_ESP32
  adc_attenuation_t attenuation_{ADC_0db};
#endif
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_ADC_COMPONENT_H
