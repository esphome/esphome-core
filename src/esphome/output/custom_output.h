#ifndef ESPHOME_OUTPUT_CUSTOM_OUTPUT_H
#define ESPHOME_OUTPUT_CUSTOM_OUTPUT_H

#include "esphome/defines.h"

#ifdef USE_CUSTOM_OUTPUT

#include "esphome/output/binary_output.h"
#include "esphome/output/float_output.h"

ESPHOME_NAMESPACE_BEGIN

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

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_CUSTOM_OUTPUT

#endif  // ESPHOME_OUTPUT_CUSTOM_OUTPUT_H
