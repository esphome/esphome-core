//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H
#define ESPHOMELIB_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H

#include "high_power_output.h"
#include "binary_output.h"
namespace esphomelib {

namespace output {

/// Simple GPIO binary output.
class GPIOBinaryOutputComponent : public BinaryOutput, public HighPowerOutput, public Component {
 public:
  explicit GPIOBinaryOutputComponent(uint8_t pin, uint8_t mode = OUTPUT);

  void setup() override;
  float get_setup_priority() const override;

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);
  uint8_t get_mode() const;
  void set_mode(uint8_t mode);

 protected:
  void write_value(bool value) override;

  uint8_t pin_;
  uint8_t mode_;
};

} // namespace output

} // namespace esphomelib

#endif //ESPHOMELIB_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H
