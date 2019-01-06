#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_OUTPUT

#include "esphomelib/output/custom_output.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace output {

CustomBinaryOutputConstructor::CustomBinaryOutputConstructor(std::function<std::vector<BinaryOutput *>()> init) {
  this->outputs_ = init();
}
BinaryOutput *CustomBinaryOutputConstructor::get_output(int i) {
  return this->outputs_[i];
}
CustomFloatOutputConstructor::CustomFloatOutputConstructor(std::function<std::vector<FloatOutput *>()> init) {
  this->outputs_ = init();
}
FloatOutput *CustomFloatOutputConstructor::get_output(int i) {
  return this->outputs_[i];
}
} // namespace output

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_OUTPUT
