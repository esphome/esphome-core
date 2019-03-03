#ifndef ESPHOME_CUSTOM_SWITCH_H
#define ESPHOME_CUSTOM_SWITCH_H

#include "esphome/defines.h"

#ifdef USE_CUSTOM_SWITCH

#include "esphome/switch_/switch.h"
#include "esphome/component.h"

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

class CustomSwitchConstructor : public Component {
 public:
  CustomSwitchConstructor(std::function<std::vector<Switch *>()> init);

  Switch *get_switch(int i);

  void dump_config() override;

 protected:
  std::vector<Switch *> switches_;
};

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_CUSTOM_SWITCH

#endif  // ESPHOME_CUSTOM_SWITCH_H
