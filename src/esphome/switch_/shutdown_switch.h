#ifndef ESPHOME_SWITCH_SHUTDOWN_SWITCH_H
#define ESPHOME_SWITCH_SHUTDOWN_SWITCH_H

#include "esphome/switch_/switch.h"
#include "esphome/defines.h"

#ifdef USE_SHUTDOWN_SWITCH

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

/// A simple switch that will put the node into deep sleep indefinitely.
class ShutdownSwitch : public Switch, public Component {
 public:
  explicit ShutdownSwitch(const std::string &name);

  std::string icon() override;

  void dump_config() override;

 protected:
  void write_state(bool state) override;
};

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_RESTART_SWITCH

#endif  // ESPHOME_SWITCH_SHUTDOWN_SWITCH_H
