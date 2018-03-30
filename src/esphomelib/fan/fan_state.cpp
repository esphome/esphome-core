//
// Created by Otto Winter on 29.12.17.
//

#include "esphomelib/fan/fan_state.h"
#include "esphomelib/esppreferences.h"

namespace esphomelib {

namespace fan {

bool FanState::get_state() const {
  return this->state_;
}
void FanState::set_state(bool state) {
  this->state_ = state;
  this->send_callback_.call();
  this->update_callback_.call();
}
FanState::Speed esphomelib::fan::FanState::get_speed() const {
  return this->speed_;
}
void FanState::set_speed(FanState::Speed speed) {
  this->speed_ = speed;
  this->send_callback_.call();
  this->update_callback_.call();
}
bool FanState::is_oscillating() const {
  return this->oscillating_;
}
void FanState::set_oscillating(bool oscillating) {
  this->oscillating_ = oscillating;
  this->send_callback_.call();
  this->update_callback_.call();
}
const FanTraits &FanState::get_traits() const {
  return this->traits_;
}
void FanState::set_traits(const FanTraits &traits) {
  this->traits_ = traits;
}
void FanState::add_on_receive_frontend_state_callback(std::function<void()> &&send_callback) {
  this->send_callback_.add(std::move(send_callback));
}
void FanState::add_on_receive_backend_state_callback(std::function<void()> &&update_callback) {
  this->update_callback_.add(std::move(update_callback));
}
void FanState::load_from_preferences(const std::string &friendly_name) {
  this->set_state(global_preferences.get_bool(friendly_name, "state", false));
  this->set_oscillating(global_preferences.get_bool(friendly_name, "oscillating", false));
  this->set_speed(static_cast<Speed>(global_preferences.get_int32(friendly_name, "speed", SPEED_HIGH)));
}
void FanState::save_to_preferences(const std::string &friendly_name) {
  global_preferences.put_bool(friendly_name, "state", this->get_state());
  global_preferences.put_bool(friendly_name, "oscillating", this->is_oscillating());
  global_preferences.put_int32(friendly_name, "speed", this->get_speed());
}

} // namespace fan

} // namespace esphomelib
