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
class Switch : public binary_sensor::BinarySensor, public Component {
 public:
  explicit Switch(const std::string &name);

  float get_setup_priority() const override;
  void setup_() override;

  /// This method is called by the front-end components.
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

 protected:
  /// Turn this switch on. When creating a switch, you should implement this (inversion will already be applied).
  virtual void turn_on() = 0;
  /// Turn this switch off. When creating a switch, you should implement this (inversion will already be applied).
  virtual void turn_off() = 0;

  optional<std::string> icon_{}; ///< The icon shown here. Not set means use default from switch. Empty means no icon.
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
  this->switch_->write_state(!this->switch_->get_value());
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
