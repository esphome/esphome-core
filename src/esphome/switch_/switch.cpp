#include "esphome/defines.h"

#ifdef USE_SWITCH

#include "esphome/switch_/switch.h"
#include "esphome/log.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch";

std::string Switch::icon() { return ""; }
Switch::Switch(const std::string &name) : Nameable(name), state(false) {}
Switch::Switch() : Switch("") {}

std::string Switch::get_icon() {
  if (this->icon_.has_value())
    return *this->icon_;
  return this->icon();
}

void Switch::set_icon(const std::string &icon) { this->icon_ = icon; }
void Switch::turn_on() {
  ESP_LOGD(TAG, "'%s' Turning ON.", this->get_name().c_str());
  this->write_state(!this->inverted_);
}
void Switch::turn_off() {
  ESP_LOGD(TAG, "'%s' Turning OFF.", this->get_name().c_str());
  this->write_state(this->inverted_);
}
void Switch::toggle() {
  ESP_LOGD(TAG, "'%s' Toggling %s.", this->get_name().c_str(), this->state ? "OFF" : "ON");
  this->write_state(this->inverted_ == this->state);
}
optional<bool> Switch::get_initial_state() {
  this->rtc_ = global_preferences.make_preference<bool>(this->get_object_id_hash());
  bool initial_state;
  if (!this->rtc_.load(&initial_state))
    return {};
  return initial_state;
}
void Switch::publish_state(bool state) {
  if (!this->publish_dedup_.next(state))
    return;
  this->state = state != this->inverted_;

  this->rtc_.save(&this->state);
  ESP_LOGD(TAG, "'%s': Sending state %s", this->name_.c_str(), ONOFF(state));
  this->state_callback_.call(this->state);
}
bool Switch::assumed_state() { return false; }

void Switch::add_on_state_callback(std::function<void(bool)> &&callback) {
  this->state_callback_.add(std::move(callback));
}
void Switch::set_inverted(bool inverted) { this->inverted_ = inverted; }
uint32_t Switch::hash_base() { return 3129890955UL; }
bool Switch::is_inverted() const { return this->inverted_; }
#ifdef USE_MQTT_SWITCH
MQTTSwitchComponent *Switch::get_mqtt() const { return this->mqtt_; }
void Switch::set_mqtt(MQTTSwitchComponent *mqtt) { this->mqtt_ = mqtt; }
#endif
SwitchTurnOnTrigger *Switch::make_switch_turn_on_trigger() { return new SwitchTurnOnTrigger(this); }
SwitchTurnOffTrigger *Switch::make_switch_turn_off_trigger() { return new SwitchTurnOffTrigger(this); }

SwitchTurnOnTrigger::SwitchTurnOnTrigger(Switch *a_switch) {
  a_switch->add_on_state_callback([this](bool state) {
    if (state) {
      this->trigger();
    }
  });
}
SwitchTurnOffTrigger::SwitchTurnOffTrigger(Switch *a_switch) {
  a_switch->add_on_state_callback([this](bool state) {
    if (!state) {
      this->trigger();
    }
  });
}
}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_SWITCH
