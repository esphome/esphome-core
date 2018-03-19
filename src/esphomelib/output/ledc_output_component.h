//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_OUTPUT_LEDC_OUTPUT_COMPONENT_H
#define ESPHOMELIB_OUTPUT_LEDC_OUTPUT_COMPONENT_H

#ifdef ARDUINO_ARCH_ESP32

#include "esphomelib/output/float_output.h"
#include "esphomelib/output/high_power_output.h"

namespace esphomelib {

namespace output {

/// ESP32 LEDC output component.
class LEDCOutputComponent : public FloatOutput, public HighPowerOutput, public Component {
 public:
  /// Construct a LEDCOutputComponent. The channel will be set using the next_ledc_channel global variable.
  explicit LEDCOutputComponent(uint8_t pin, float frequency = 1000.0f, uint8_t bit_depth = 12);

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);
  uint8_t get_channel() const;
  void set_channel(uint8_t channel);
  uint8_t get_bit_depth() const;
  void set_bit_depth(uint8_t bit_depth);
  float get_frequency() const;
  void set_frequency(float frequency);

  void write_value_f(float adjusted_value) override;

  void setup() override;
  float get_setup_priority() const override;

 protected:
  uint8_t pin_;
  uint8_t channel_;
  uint8_t bit_depth_;
  float frequency_;
};

extern uint8_t next_ledc_channel;

} // namespace output

} // namespace esphomelib

#endif

#endif //ESPHOMELIB_OUTPUT_LEDC_OUTPUT_COMPONENT_H
