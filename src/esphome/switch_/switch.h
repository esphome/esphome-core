#ifndef ESPHOME_SWITCH_SWITCH_H
#define ESPHOME_SWITCH_SWITCH_H

#include "esphome/defines.h"

#ifdef USE_SWITCH

#include "esphome/component.h"
#include "esphome/automation.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

template<typename... Ts> class ToggleAction;
template<typename... Ts> class TurnOffAction;
template<typename... Ts> class TurnOnAction;
template<typename... Ts> class SwitchCondition;
template<typename... Ts> class SwitchPublishAction;
class SwitchTurnOnTrigger;
class SwitchTurnOffTrigger;

#define LOG_SWITCH(prefix, type, obj) \
  if (obj != nullptr) { \
    ESP_LOGCONFIG(TAG, prefix type " '%s'", obj->get_name().c_str()); \
    if (!obj->get_icon().empty()) { \
      ESP_LOGCONFIG(TAG, prefix "  Icon: '%s'", obj->get_icon().c_str()); \
    } \
    if (obj->assumed_state()) { \
      ESP_LOGCONFIG(TAG, prefix "  Assumed State: YES"); \
    } \
    if (obj->is_inverted()) { \
      ESP_LOGCONFIG(TAG, prefix "  Inverted: YES"); \
    } \
  }

#ifdef USE_MQTT_SWITCH
class MQTTSwitchComponent;
#endif

/** Base class for all switches.
 *
 * A switch is basically just a combination of a binary sensor (for reporting switch values)
 * and a write_state method that writes a state to the hardware.
 */
class Switch : public Nameable {
 public:
  explicit Switch();
  explicit Switch(const std::string &name);

  /** Publish a state to the front-end from the back-end.
   *
   * The input value is inverted if applicable. Then the internal value member is set and
   * finally the callbacks are called.
   *
   * @param state The new state.
   */
  void publish_state(bool state);

  union {
    /// The current reported state of the binary sensor.
    bool state;
    ESPDEPRECATED(".value is deprecated, please use .state instead") bool value;
  };

  /** Turn this switch on. This is called by the front-end.
   *
   * For implementing switches, please override write_state.
   */
  void turn_on();
  /** Turn this switch off. This is called by the front-end.
   *
   * For implementing switches, please override write_state.
   */
  void turn_off();
  /** Toggle this switch. This is called by the front-end.
   *
   * For implementing switches, please override write_state.
   */
  void toggle();

  /** Set whether the state should be treated as inverted.
   *
   * To the developer and user an inverted switch will act just like a non-inverted one.
   * In particular, the only thing that's changed by this is the value passed to
   * write_state and the state in publish_state. The .state member variable and
   * turn_on/turn_off/toggle remain unaffected.
   *
   * @param inverted Whether to invert this switch.
   */
  void set_inverted(bool inverted);

  /// Set the icon for this switch. "" for no icon.
  void set_icon(const std::string &icon);

  /// Get the icon for this switch. Using icon() if not manually set
  std::string get_icon();

  template<typename... Ts> ToggleAction<Ts...> *make_toggle_action();
  template<typename... Ts> TurnOffAction<Ts...> *make_turn_off_action();
  template<typename... Ts> TurnOnAction<Ts...> *make_turn_on_action();
  template<typename... Ts> SwitchCondition<Ts...> *make_switch_is_on_condition();
  template<typename... Ts> SwitchCondition<Ts...> *make_switch_is_off_condition();
  template<typename... Ts> SwitchPublishAction<Ts...> *make_switch_publish_action();
  SwitchTurnOnTrigger *make_switch_turn_on_trigger();
  SwitchTurnOffTrigger *make_switch_turn_off_trigger();

  /** Set callback for state changes.
   *
   * @param callback The void(bool) callback.
   */
  void add_on_state_callback(std::function<void(bool)> &&callback);

  optional<bool> get_initial_state();

  /** Return whether this switch uses an assumed state - i.e. if both the ON/OFF actions should be displayed in Home
   * Assistant because the real state is unknown.
   *
   * Defaults to false.
   */
  virtual bool assumed_state();

  bool is_inverted() const;

#ifdef USE_MQTT_SWITCH
  MQTTSwitchComponent *get_mqtt() const;
  void set_mqtt(MQTTSwitchComponent *mqtt);
#endif

 protected:
  /** Write the given state to hardware. You should implement this
   * abstract method if you want to create your own switch.
   *
   * In the implementation of this method, you should also call
   * publish_state to acknowledge that the state was written to the hardware.
   *
   * @param state The state to write. Inversion is already applied if user specified it.
   */
  virtual void write_state(bool state) = 0;

  /** Override this to set the Home Assistant icon for this switch.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this switch, for example "mdi:fan".
   */
  virtual std::string icon();  // NOLINT

  uint32_t hash_base() override;

  optional<std::string> icon_{};  ///< The icon shown here. Not set means use default from switch. Empty means no icon.

  CallbackManager<void(bool)> state_callback_{};
  bool inverted_{false};
  Deduplicator<bool> publish_dedup_;
  ESPPreferenceObject rtc_;
#ifdef USE_MQTT_SWITCH
  MQTTSwitchComponent *mqtt_{nullptr};
#endif
};

template<typename... Ts> class TurnOnAction : public Action<Ts...> {
 public:
  explicit TurnOnAction(Switch *a_switch);

  void play(Ts... x) override;

 protected:
  Switch *switch_;
};

template<typename... Ts> class TurnOffAction : public Action<Ts...> {
 public:
  explicit TurnOffAction(Switch *a_switch);

  void play(Ts... x) override;

 protected:
  Switch *switch_;
};

template<typename... Ts> class ToggleAction : public Action<Ts...> {
 public:
  explicit ToggleAction(Switch *a_switch);

  void play(Ts... x) override;

 protected:
  Switch *switch_;
};

template<typename... Ts> class SwitchCondition : public Condition<Ts...> {
 public:
  SwitchCondition(Switch *parent, bool state);
  bool check(Ts... x) override;

 protected:
  Switch *parent_;
  bool state_;
};

class SwitchTurnOnTrigger : public Trigger<> {
 public:
  SwitchTurnOnTrigger(Switch *a_switch);
};

class SwitchTurnOffTrigger : public Trigger<> {
 public:
  SwitchTurnOffTrigger(Switch *a_switch);
};

template<typename... Ts> class SwitchPublishAction : public Action<Ts...> {
 public:
  SwitchPublishAction(Switch *a_switch);
  template<typename V> void set_state(V state) { this->state_ = state; }
  void play(Ts... x) override;

 protected:
  Switch *switch_;
  TemplatableValue<bool, Ts...> state_;
};

// =============== TEMPLATE DEFINITIONS ===============

template<typename... Ts> TurnOnAction<Ts...>::TurnOnAction(Switch *a_switch) : switch_(a_switch) {}

template<typename... Ts> void TurnOnAction<Ts...>::play(Ts... x) {
  this->switch_->turn_on();
  this->play_next(x...);
}

template<typename... Ts> TurnOffAction<Ts...>::TurnOffAction(Switch *a_switch) : switch_(a_switch) {}

template<typename... Ts> void TurnOffAction<Ts...>::play(Ts... x) {
  this->switch_->turn_off();
  this->play_next(x...);
}

template<typename... Ts> ToggleAction<Ts...>::ToggleAction(Switch *a_switch) : switch_(a_switch) {}

template<typename... Ts> void ToggleAction<Ts...>::play(Ts... x) {
  this->switch_->toggle();
  this->play_next(x...);
}

template<typename... Ts> ToggleAction<Ts...> *Switch::make_toggle_action() { return new ToggleAction<Ts...>(this); }
template<typename... Ts> TurnOffAction<Ts...> *Switch::make_turn_off_action() { return new TurnOffAction<Ts...>(this); }
template<typename... Ts> TurnOnAction<Ts...> *Switch::make_turn_on_action() { return new TurnOnAction<Ts...>(this); }

template<typename... Ts>
SwitchCondition<Ts...>::SwitchCondition(Switch *parent, bool state) : parent_(parent), state_(state) {}
template<typename... Ts> bool SwitchCondition<Ts...>::check(Ts... x) { return this->parent_->state == this->state_; }

template<typename... Ts> SwitchCondition<Ts...> *Switch::make_switch_is_on_condition() {
  return new SwitchCondition<Ts...>(this, true);
}
template<typename... Ts> SwitchCondition<Ts...> *Switch::make_switch_is_off_condition() {
  return new SwitchCondition<Ts...>(this, false);
}

template<typename... Ts> SwitchPublishAction<Ts...>::SwitchPublishAction(Switch *a_switch) : switch_(a_switch) {}
template<typename... Ts> void SwitchPublishAction<Ts...>::play(Ts... x) {
  this->switch_->publish_state(this->state_.value(x...));
  this->play_next(x...);
}
template<typename... Ts> SwitchPublishAction<Ts...> *Switch::make_switch_publish_action() {
  return new SwitchPublishAction<Ts...>(this);
}

}  // namespace switch_

ESPHOME_NAMESPACE_END

#include "esphome/switch_/mqtt_switch_component.h"

#endif  // USE_SWITCH

#endif  // ESPHOME_SWITCH_SWITCH_H
