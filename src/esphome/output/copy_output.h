#ifndef ESPHOME_CORE_OUTPUT_COPY_OUTPUT_H
#define ESPHOME_CORE_OUTPUT_COPY_OUTPUT_H

#include "esphome/defines.h"

#ifdef USE_COPY_OUTPUT

#include "esphome/component.h"
#include "esphome/output/binary_output.h"
#include "esphome/output/float_output.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

class BinaryCopyOutput : public BinaryOutput {
 public:
  BinaryCopyOutput(const std::vector<BinaryOutput *> &outputs);

 protected:
  std::vector<BinaryOutput *> outputs_;

  void write_state(bool state) override;
};

class FloatCopyOutput : public FloatOutput {
 public:
  FloatCopyOutput(const std::vector<FloatOutput *> &outputs);

 protected:
  std::vector<FloatOutput *> outputs_;

  void write_state(float state) override;
};

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_COPY_OUTPUT

#endif  // ESPHOME_CORE_OUTPUT_COPY_OUTPUT_H
