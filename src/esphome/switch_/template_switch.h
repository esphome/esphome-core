#ifndef ESPHOME_SWITCH_TEMPLATE_SWITCH_H
#define ESPHOME_SWITCH_TEMPLATE_SWITCH_H

#include "esphome/defines.h"

#ifdef USE_TEMPLATE_SWITCH

#include "esphome/switch_/switch.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

class TemplateSwitch : public Switch, public Component {
 public:
  explicit TemplateSwitch(const std::string &name);

  void setup() override;
  void dump_config() override;

  void set_state_lambda(std::function<optional<bool>()> &&f);
  void set_restore_state(bool restore_state);
  Trigger<> *get_turn_on_trigger() const;
  Trigger<> *get_turn_off_trigger() const;
  void set_optimistic(bool optimistic);
  void set_assumed_state(bool assumed_state);
  void loop() override;

  float get_setup_priority() const override;

 protected:
  bool assumed_state() override;

  void write_state(bool state) override;

  optional<std::function<optional<bool>()>> f_;
  bool optimistic_{false};
  bool assumed_state_{false};
  Trigger<> *turn_on_trigger_;
  Trigger<> *turn_off_trigger_;
  Trigger<> *prev_trigger_{nullptr};
  bool restore_state_{false};
};

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_SWITCH

#endif  // ESPHOME_SWITCH_TEMPLATE_SWITCH_H
