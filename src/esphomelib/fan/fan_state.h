//
// Created by Otto Winter on 29.12.17.
//

#ifndef ESPHOMELIB_FAN_FAN_STATE_H
#define ESPHOMELIB_FAN_FAN_STATE_H

#include <functional>

#include "esphomelib/helpers.h"
#include "esphomelib/component.h"
#include "esphomelib/fan/fan_traits.h"
#include "esphomelib/defines.h"

#ifdef USE_FAN

namespace esphomelib {

namespace fan {

/** FanState - This class is shared between the hardware backend and the MQTT frontend to share state.
 *
 * A fan state has several variables that determine the current state: state (ON/OFF),
 * speed (OFF, LOW, MEDIUM, HIGH), oscillating (ON/OFF) and traits (what features are supported).
 * Both the frontend and the backend can register callbacks whenever a state is changed from the
 * frontend and whenever a state is actually changed and should be pushed to the frontend
 */
class FanState : public Nameable {
 public:
  /// Simple enum to represent the speed of
  enum Speed {
    SPEED_OFF = 0, ///< The fan is OFF (this option combined with state ON should make the fan be off.)
    SPEED_LOW, ///< The fan is running on low speed.
    SPEED_MEDIUM, ///< The fan is running on medium speed.
    SPEED_HIGH  ///< The fan is running on high/full speed.
  };

  explicit FanState(const std::string &name);

  /// Register a callback that will be called each time the frontend wants to set a state.
  void add_on_receive_frontend_state_callback(std::function<void()> &&send_callback);
  /// Register a callback that will be called each time the backend indicates a state change.
  void add_on_receive_backend_state_callback(std::function<void()> &&update_callback);

  /// Get the current ON/OFF state of this fan.
  bool get_state() const;
  /// Set the current ON/OFF state of this fan.
  void set_state(bool state);
  /// Get the current oscillating state of this fan.
  bool is_oscillating() const;
  /// Set the current oscillating state of this fan.
  void set_oscillating(bool oscillating);
  /// Get the current speed of this fan.
  Speed get_speed() const;
  /// Set the current speed of this fan.
  void set_speed(Speed speed);
  bool set_speed(const char *speed);
  /// Get the traits of this fan (i.e. what features it supports).
  const FanTraits &get_traits() const;
  /// Set the traits of this fan (i.e. what features it supports).
  void set_traits(const FanTraits &traits);

  /// Load a fan state from the preferences into this object.
  void load_from_preferences();
  /// Save the fan state from this object into the preferences.
  void save_to_preferences();

 protected:
  bool state_{false};
  bool oscillating_{false};
  Speed speed_{SPEED_HIGH};
  FanTraits traits_{};
  CallbackManager<void()> send_callback_{};
  CallbackManager<void()> update_callback_{};
};

} // namespace fan

} // namespace esphomelib

#endif //USE_FAN

#endif //ESPHOMELIB_FAN_FAN_STATE_H
