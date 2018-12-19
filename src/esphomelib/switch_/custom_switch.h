#ifndef ESPHOMELIB_CUSTOM_SWITCH_H
#define ESPHOMELIB_CUSTOM_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_SWITCH

#include "esphomelib/switch_/switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

class CustomSwitchConstructor {
 public:
  CustomSwitchConstructor(std::function<std::vector<Switch *>()> init);

  Switch *get_switch(int i);
 protected:
  std::vector<Switch *> switches_;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_SWITCH

#endif //ESPHOMELIB_CUSTOM_SWITCH_H
