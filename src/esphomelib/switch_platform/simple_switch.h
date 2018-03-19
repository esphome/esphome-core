//
// Created by Otto Winter on 20.01.18.
//

#ifndef ESPHOMELIB_SWITCH_PLATFORM_SIMPLE_SWITCH
#define ESPHOMELIB_SWITCH_PLATFORM_SIMPLE_SWITCH

#include "esphomelib/output/binary_output.h"
#include "esphomelib/switch_platform/switch.h"

namespace esphomelib {

namespace switch_platform {

class SimpleSwitch : public Switch {
 public:
  explicit SimpleSwitch(output::BinaryOutput *output);

  void write_state(bool state) override;
 protected:
  output::BinaryOutput *output_;
};

} // namespace switch_platform

} // namespace esphomelib

#endif //ESPHOMELIB_SWITCH_PLATFORM_SIMPLE_SWITCH
