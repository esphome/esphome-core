//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_OUTPUT_FLOAT_OUTPUT_H
#define ESPHOMELIB_OUTPUT_FLOAT_OUTPUT_H

#include <esphomelib/atx_component.h>
#include "esphomelib/component.h"
#include "binary_output.h"

namespace esphomelib {

namespace output {

/// Output of float values.
class FloatOutput : public BinaryOutput {
 public:
  FloatOutput();

  /// Use this to set the FloatOutput to a new value.
  void set_value_f(float value);

  float get_max_power() const;
  /// Set the maximum power output of this component. All values are multiplied by this float to get the adjusted value.
  void set_max_power(float max_power);

  /// Override the BinaryOutput set_value for floating point output.
  void set_value(bool value) override;

 protected:
  void write_value(bool value) override;

  /// This will be called with the adjusted (max_power) value. Note: you should handle inverted outputs yourself.
  virtual void write_value_f(float adjusted_value) = 0;

  float max_power_;
};

} // namespace output

} // namespace esphomelib


#endif //ESPHOMELIB_OUTPUT_FLOAT_OUTPUT_H
