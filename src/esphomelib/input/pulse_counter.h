//
//  pulse_counter.h
//  esphomelib
//
//  Created by Otto Winter on 24.02.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_INPUT_PULSE_COUNTER_H
#define ESPHOMELIB_INPUT_PULSE_COUNTER_H

#ifdef ARDUINO_ARCH_ESP32 // only available on ESP32

#include <driver/pcnt.h>

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

namespace input {

/** Pulse Counter - This is the sensor component for the ESP32 integrated pulse counter peripheral.
 *
 * It offers 8 pulse counter units that can be setup in several ways to count pulses on a pin.
 * Also allows for some simple filtering of short pulses using set_filter(), any pulse shorter than
 * the value provided to that function will be discarded. The time is given in APB clock cycles,
 * which usually amount to 12.5 ns per clock. Defaults to the max possible (about 13 ms).
 * See http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/pcnt.html for more information.
 *
 * The pulse counter defaults to reporting a value of the measurement unit "pulses/min". To
 * modify this behavior, use filters in MQTTSensor.
 */
class PulseCounterSensorComponent : public Component, public sensor::Sensor {
 public:
  /** Construct the Pulse Counter instance with the provided pin and update interval.
   *
   * The pulse counter unit will automatically be set and the pulse counter is set up
   * to increment the counter on rising edges by default.
   *
   * @param pin The pin.
   * @param update_interval The update interval in ms.
   */
  explicit PulseCounterSensorComponent(uint8_t pin, uint32_t update_interval = 30000);

  /// Manually set the pin for the pulse counter unit.
  void set_pin(uint8_t pin);

  /// Manually set the pull mode of this pin, default to floating.
  void set_pull_mode(gpio_pull_mode_t pull_mode);

  /// Set the pcnt_count_mode_t for the rising and falling edges. can be disable, increment and decrement.
  void set_edge_mode(pcnt_count_mode_t rising_edge_mode, pcnt_count_mode_t falling_edge_mode);

  /** Set a filter for this Pulse Counter unit.
   *
   * See http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/pcnt.html#filtering-pulses
   *
   * Filter is given in APB clock cycles, so a value of one would filter out any pulses
   * shorter than 12.5 ns. This value can have 10-bit at maximum, so the maximum possible
   * value is 1023, or about 12ms.
   *
   * @param filter The filter length in APB clock cycles.
   */
  void set_filter(uint16_t filter);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Manually set the pulse counter unit to be used. This is automatically set by the constructor.
  void set_pcnt_unit(pcnt_unit_t pcnt_unit);

  /// Return the value from set_filter().
  uint16_t get_filter() const;

  /// Return the pull mode from set_pull_mode():
  gpio_pull_mode_t get_pull_mode() const;

  /// Get the pulse counter unit of this component. Automatically set by constructor.
  pcnt_unit_t get_pcnt_unit() const;

  pcnt_count_mode_t get_rising_edge_mode() const;
  pcnt_count_mode_t get_falling_edge_mode() const;

  std::string unit_of_measurement() override;
  void setup() override;
  float get_setup_priority() const override;
  uint8_t get_pin() const;

 protected:
  uint8_t pin_;
  gpio_pull_mode_t pull_mode_{GPIO_FLOATING};
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
