#include "esphome/defines.h"

#ifdef USE_COPY_OUTPUT

#include "esphome/output/copy_output.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

BinaryCopyOutput::BinaryCopyOutput(const std::vector<BinaryOutput *> &outputs) : outputs_(outputs) {}
void BinaryCopyOutput::write_state(bool state) {
  for (auto *out : this->outputs_) {
    if (state)
      out->turn_on();
    else
      out->turn_off();
  }
}
FloatCopyOutput::FloatCopyOutput(const std::vector<FloatOutput *> &outputs) : outputs_(outputs) {}
void FloatCopyOutput::write_state(float state) {
  for (auto *out : this->outputs_) {
    out->set_level(state);
  }
}

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_COPY_OUTPUT
