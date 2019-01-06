#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_SWITCH

#include "esphomelib/switch_/custom_switch.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.custom";

CustomSwitchConstructor::CustomSwitchConstructor(std::function<std::vector<Switch *>()> init) {
  this->switches_ = init();
}
Switch *CustomSwitchConstructor::get_switch(int i) {
  return this->switches_[i];
}
void CustomSwitchConstructor::dump_config() {
  for (auto *child : this->switches_) {
    LOG_SWITCH("", "Custom Switch", child);
  }
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_SWITCH
