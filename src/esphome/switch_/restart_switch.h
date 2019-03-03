#ifndef ESPHOME_RESTART_SWITCH_H
#define ESPHOME_RESTART_SWITCH_H

#include "esphome/switch_/switch.h"
#include "esphome/defines.h"

#ifdef USE_RESTART_SWITCH

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

/// A simple switch that restarts the device when triggered.
class RestartSwitch : public Switch, public Component {
 public:
  explicit RestartSwitch(const std::string &name);

  std::string icon() override;

  void dump_config() override;

 protected:
  void write_state(bool state) override;
};

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_RESTART_SWITCH

#endif  // ESPHOME_RESTART_SWITCH_H
