#ifndef ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H
#define ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SWITCH

#include "esphomelib/switch_/switch.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

class TemplateSwitch : public Switch, public Component {
 public:
  explicit TemplateSwitch(const std::string &name);

  void setup() override;
  void dump_config() override;

  void set_state_lambda(std::function<optional<bool>()> &&f);
  void set_restore_state(bool restore_state);
  Trigger<NoArg> *get_turn_on_trigger() const;
  Trigger<NoArg> *get_turn_off_trigger() const;
  void set_optimistic(bool optimistic);
  void set_assumed_state(bool assumed_state);
  void loop() override;

  float get_setup_priority() const override;

 protected:
  bool assumed_state() override;

  void write_state(bool state) override;

  optional<std::function<optional<bool>()>> f_;
  bool optimistic_{false};
  optional<bool> last_state_{};
  bool assumed_state_{false};
  Trigger<NoArg> *turn_on_trigger_;
  Trigger<NoArg> *turn_off_trigger_;
  Trigger<NoArg> *prev_trigger_{nullptr};
  bool restore_state_{true};
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SWITCH

#endif //ESPHOMELIB_SWITCH_TEMPLATE_SWITCH_H
