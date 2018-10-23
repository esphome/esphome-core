#ifndef ESPHOMELIB_SWITCH_SHUTDOWN_SWITCH_H
#define ESPHOMELIB_SWITCH_SHUTDOWN_SWITCH_H

#include "esphomelib/switch_/switch.h"
#include "esphomelib/defines.h"

#ifdef USE_SHUTDOWN_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

/// A simple switch that will put the node into deep sleep indefinitely.
class ShutdownSwitch : public Switch {
 public:
  explicit ShutdownSwitch(const std::string &name);

  std::string icon() override;
 protected:
  void write_state(bool state) override;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_RESTART_SWITCH


#endif //ESPHOMELIB_SWITCH_SHUTDOWN_SWITCH_H
