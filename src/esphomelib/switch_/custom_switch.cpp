#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_SWITCH

#include "esphomelib/switch_/custom_switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

CustomSwitchConstructor::CustomSwitchConstructor(std::function<std::vector<Switch *>()> init) {
  this->switches_ = init();
}
Switch *CustomSwitchConstructor::get_switch(int i) {
  return this->switches_[i];
}
} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_SWITCH
