#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_SWITCH

#include "esphomelib/switch_/template_switch.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.template";

TemplateSwitch::TemplateSwitch(const std::string &name)
    : Switch(name), Component(), turn_on_trigger_(new Trigger<NoArg>()), turn_off_trigger_(new Trigger<NoArg>()) {

}
void TemplateSwitch::loop() {
  if (!this->f_.has_value())
    return;
  auto s = (*this->f_)();
  if (!s.has_value())
    return;

  if (this->last_state_.value_or(!*s) == *s)
    return;

  this->last_state_ = *s;
  this->publish_state(*s);
}
void TemplateSwitch::write_state(bool state) {
  if (state) {
    this->turn_on_trigger_->trigger();
  } else {
    this->turn_off_trigger_->trigger();
  }

  if (this->optimistic_)
    this->publish_state(state);
}
void TemplateSwitch::set_optimistic(bool optimistic) {
  this->optimistic_ = optimistic;
}
bool TemplateSwitch::optimistic() {
  return this->optimistic_;
}
void TemplateSwitch::set_state_lambda(std::function<optional<bool>()> &&f) {
  this->f_ = f;
}
float TemplateSwitch::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
Trigger<NoArg> *TemplateSwitch::get_turn_on_trigger() const {
  return this->turn_on_trigger_;
}
Trigger<NoArg> *TemplateSwitch::get_turn_off_trigger() const {
  return this->turn_off_trigger_;
}
void TemplateSwitch::setup() {
  if (!this->restore_state_)
    return;

  auto restored = this->get_initial_state();
  if (!restored.has_value())
    return;

  ESP_LOGD(TAG, "  Restored state %s", ONOFF(*restored));
  if (*restored) {
    this->turn_on();
  } else {
    this->turn_off();
  }
}
void TemplateSwitch::dump_config() {
  LOG_SWITCH("", "Template Switch", this);
  ESP_LOGCONFIG(TAG, "  Restore State: %s", YESNO(this->restore_state_));
}
void TemplateSwitch::set_restore_state(bool restore_state) {
  this->restore_state_ = restore_state;
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_SWITCH
