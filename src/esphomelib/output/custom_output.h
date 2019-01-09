#ifndef ESPHOMELIB_OUTPUT_CUSTOM_OUTPUT_H
#define ESPHOMELIB_OUTPUT_CUSTOM_OUTPUT_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_OUTPUT

#include "esphomelib/output/binary_output.h"
#include "esphomelib/output/float_output.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace output {

class CustomBinaryOutputConstructor {
 public:
  CustomBinaryOutputConstructor(std::function<std::vector<BinaryOutput *>()> init);

  BinaryOutput *get_output(int i);

 protected:
  std::vector<BinaryOutput *> outputs_;
};

class CustomFloatOutputConstructor {
 public:
  CustomFloatOutputConstructor(std::function<std::vector<FloatOutput *>()> init);

  FloatOutput *get_output(int i);

 protected:
  std::vector<FloatOutput *> outputs_;
};

} // namespace output

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_OUTPUT

#endif //ESPHOMELIB_OUTPUT_CUSTOM_OUTPUT_H
