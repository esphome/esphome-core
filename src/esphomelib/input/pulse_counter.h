//
//  pulse_counter.h
//  esphomelib
//
//  Created by Otto Winter on 24.02.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_INPUT_PULSE_COUNTER_H
#define ESPHOMELIB_INPUT_PULSE_COUNTER_H

#ifdef ARDUINO_ARCH_ESP32

#include <esphomelib/sensor/sensor.h>
#include <driver/pcnt.h>
#include <esphomelib/hal.h>

namespace esphomelib {

namespace input {

class PulseCounterSensorComponent : public Component, public sensor::Sensor {
 public:
  explicit PulseCounterSensorComponent(uint8_t pin, uint32_t interval = 30000);

  std::string unit_of_measurement() override;

  void setup() override;
  float get_setup_priority() const override;

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);

  uint8_t get_pin_mode() const;
  void set_pin_mode(uint8_t pin_mode);

  uint32_t get_check_interval() const;
  void set_check_interval(uint32_t check_interval);

  float get_multiplier() const;
  void set_multiplier(float multiplier);

  pcnt_unit_t get_pcnt_unit() const;
  void set_pcnt_unit(pcnt_unit_t pcnt_unit);

  pcnt_count_mode_t get_rising_edge_mode() const;
  pcnt_count_mode_t get_falling_edge_mode() const;
  void set_edge_mode(pcnt_count_mode_t rising_edge_mode, pcnt_count_mode_t falling_edge_mode);

  uint16_t get_filter() const;
  void set_filter(uint16_t filter);
 protected:
  uint8_t pin_;
  uint8_t pin_mode_{ANALOG};
  uint32_t check_interval_;
  float multiplier_{1.0f};
  pcnt_unit_t pcnt_unit_;
  pcnt_count_mode_t rising_edge_mode_{PCNT_COUNT_INC};
  pcnt_count_mode_t falling_edge_mode_{PCNT_COUNT_DIS};
  uint16_t filter_{1023};
  int16_t last_value_{0};
};

extern pcnt_unit_t next_pcnt_unit;

} // namespace input

} // namespace esphomelib

#endif //ARDUINO_ARCH_ESP32

#endif //ESPHOMELIB_INPUT_PULSE_COUNTER_H
