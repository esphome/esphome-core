//
// Created by Otto Winter on 29.12.17.
//

#ifndef ESPHOMELIB_FAN_FAN_STATE_H
#define ESPHOMELIB_FAN_FAN_STATE_H

#include <functional>
#include <vector>

#include "esphomelib/helpers.h"
#include "esphomelib/fan/fan_traits.h"

namespace esphomelib {

namespace fan {

using fan_send_callback_t = std::function<void()>;

class FanState {
 public:
  enum Speed { SPEED_OFF = 0, SPEED_LOW, SPEED_MEDIUM, SPEED_HIGH };

  void add_send_callback(const fan_send_callback_t &send_callback);
  void add_update_callback(const fan_send_callback_t &update_callback);

  bool get_state() const;
  void set_state(bool state);
  bool is_oscillating() const;
  void set_oscillating(bool oscillating);
  Speed get_speed() const;
  void set_speed(Speed speed);
  const FanTraits &get_traits() const;
  void set_traits(const FanTraits &traits);

  void load_from_preferences(const std::string &friendly_name);
  void save_to_preferences(const std::string &friendly_name);

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

#endif //ESPHOMELIB_FAN_FAN_STATE_H
