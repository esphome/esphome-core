#ifndef ESPHOMELIB_SWITCH_GPIO_SWITCH_H
#define ESPHOMELIB_SWITCH_GPIO_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_GPIO_SWITCH

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/switch_/switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

enum GPIOSwitchRestoreMode {
  GPIO_SWITCH_RESTORE_DEFAULT_OFF,
  GPIO_SWITCH_RESTORE_DEFAULT_ON,
  GPIO_SWITCH_ALWAYS_OFF,
  GPIO_SWITCH_ALWAYS_ON,
};

class GPIOSwitch : public Switch, public Component {
 public:
  GPIOSwitch(const std::string &name, GPIOPin *pin);

  void set_restore_mode(GPIOSwitchRestoreMode restore_mode);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  float get_setup_priority() const override;

  void setup() override;
  void dump_config() override;

 protected:
  void write_state(bool state) override;

  GPIOPin *const pin_;
  GPIOSwitchRestoreMode restore_mode_{GPIO_SWITCH_RESTORE_DEFAULT_OFF};
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_SWITCH

#endif //ESPHOMELIB_SWITCH_GPIO_SWITCH_H
