//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_SWITCH_SWITCH_H
#define ESPHOMELIB_SWITCH_SWITCH_H

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/component.h"
#include "esphomelib/automation.h"
#include "esphomelib/defines.h"

#ifdef USE_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

template<typename T>
class ToggleAction;
template<typename T>
class TurnOffAction;
template<typename T>
class TurnOnAction;

/** Base class for all switches.
 *
 * A switch is basically just a combination of a binary sensor (for reporting switch values)
 * and a write_state method that writes a state to the hardware.
 */
class Switch : public Component, public Nameable {
 public:
  explicit Switch(const std::string &name);

  float get_setup_priority() const override;
  void setup_() override;

  /** Publish a state to the front-end from the back-end.
   *
   * The input value is inverted if applicable. Then the internal value member is set and
   * finally the callbacks are called.
   *
   * @param state
   */
  void publish_state(bool state);

  void set_inverted(bool inverted);

  /** This method is called by the front-end components to set the state.
   *
   * The internal logic will then invert it if applicable and call the
   * turn_on or turn_off abstract method.
   *
   * Note that this will not trigger sending the state. The overriden
   * turn_on and turn_off methods should do this manually.
   *
   * @param state The binary state to write.
   */
  void write_state(bool state);

  /** Override this to set the Home Assistant icon for this switch.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this switch, for example "mdi:fan".
   */
  virtual std::string icon();

  /// Set the icon for this switch. "" for no icon.
  void set_icon(const std::string &icon);

  /// Get the icon for this switch. Using icon() if not manually set
  std::string get_icon();

  template<typename T>
  ToggleAction<T> *make_toggle_action();
  template<typename T>
  TurnOffAction<T> *make_turn_off_action();
  template<typename T>
  TurnOnAction<T> *make_turn_on_action();

  /** Return whether this switch is optimistic - i.e. if both the ON/OFF actions should be displayed in Home Assistant
   * because the real state is unknown.
   *
   * Defaults to false.
   */
  virtual bool optimistic();

  /** Set callback for state changes.
   *
   * @param callback The void(bool) callback.
   */
  virtual void add_on_state_callback(std::function<void(bool)> &&callback);

  /// Subclasses can override this to prevent the switch from automatically restoring the state.
  virtual bool do_restore_state();

  /** The current published state of the switch.
   *
   * Inversion is already applied on this value.
   */
  bool value{false};

 protected:
  /// Turn this switch on. When creating a switch, you should implement this (inversion will already be applied).
  virtual void turn_on() = 0;
  /// Turn this switch off. When creating a switch, you should implement this (inversion will already be applied).
  virtual void turn_off() = 0;

  optional<std::string> icon_{}; ///< The icon shown here. Not set means use default from switch. Empty means no icon.

  CallbackManager<void(bool)> state_callback_{};
  bool inverted_{false};
  bool first_value_{true};
};

template<typename T>
class TurnOnAction : public Action<T> {
 public:
  explicit TurnOnAction(Switch *a_switch);

  void play(T x) override;

 protected:
  Switch *switch_;
};

template<typename T>
class TurnOffAction : public Action<T> {
 public:
  explicit TurnOffAction(Switch *a_switch);

  void play(T x) override;

 protected:
  Switch *switch_;
};

template<typename T>
class ToggleAction : public Action<T> {
 public:
  explicit ToggleAction(Switch *a_switch);

  void play(T x) override;

 protected:
  Switch *switch_;
};

// =============== TEMPLATE DEFINITIONS ===============

template<typename T>
TurnOnAction<T>::TurnOnAction(Switch *a_switch) : switch_(a_switch) {}

template<typename T>
void TurnOnAction<T>::play(T x) {
  this->switch_->write_state(true);
  this->play_next(x);
}

template<typename T>
TurnOffAction<T>::TurnOffAction(Switch *a_switch) : switch_(a_switch) {}

template<typename T>
void TurnOffAction<T>::play(T x) {
  this->switch_->write_state(false);
  this->play_next(x);
}

template<typename T>
ToggleAction<T>::ToggleAction(Switch *a_switch) : switch_(a_switch) {}

template<typename T>
void ToggleAction<T>::play(T x) {
  this->switch_->write_state(!this->switch_->value);
  this->play_next(x);
}

template<typename T>
ToggleAction<T> *Switch::make_toggle_action() {
  return new ToggleAction<T>(this);
}

template<typename T>
TurnOffAction<T> *Switch::make_turn_off_action() {
  return new TurnOffAction<T>(this);
}

template<typename T>
TurnOnAction<T> *Switch::make_turn_on_action() {
  return new TurnOnAction<T>(this);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH

#endif //ESPHOMELIB_SWITCH_SWITCH_H
