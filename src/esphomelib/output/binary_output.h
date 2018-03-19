//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_OUTPUT_BINARY_OUTPUT_H
#define ESPHOMELIB_OUTPUT_BINARY_OUTPUT_H

#include "esphomelib/switch_platform/switch.h"

namespace esphomelib {

namespace output {

/// Simple binary output base-class. Allows inverted outputs.
class BinaryOutput {
 public:
  BinaryOutput();

  virtual void set_value(bool value);

  bool is_inverted() const;
  void set_inverted(bool inverted);

 protected:
  /// Override this method to write the provided value (inversion already applied) to hardware.
  virtual void write_value(bool value) = 0;

  bool inverted_;
};

} // namespace output

} // namespace esphomelib

#endif //ESPHOMELIB_OUTPUT_BINARY_OUTPUT_H
