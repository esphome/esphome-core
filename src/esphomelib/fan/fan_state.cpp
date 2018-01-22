//
// Created by Otto Winter on 29.12.17.
//

#include "fan_state.h"

namespace esphomelib {

namespace fan {

bool FanState::get_state() const {
  return this->state_;
}
void FanState::set_state(bool state) {
  this->state_ = state;
  if (this->send_callback_)
    this->send_callback_();
  if (this->update_callback_)
    this->update_callback_();
}
FanState::Speed esphomelib::fan::FanState::get_speed() const {
  return this->speed_;
}
void FanState::set_speed(FanState::Speed speed) {
  this->speed_ = speed;
  if (this->send_callback_)
    this->send_callback_();
  if (this->update_callback_)
    this->update_callback_();
}
bool FanState::is_oscillating() const {
  return this->oscillating_;
}
void FanState::set_oscillating(bool oscillating) {
  this->oscillating_ = oscillating;
  if (this->send_callback_)
    this->send_callback_();
  if (this->update_callback_)
    this->update_callback_();
}
const FanTraits &FanState::get_traits() const {
  return this->traits_;
}
void FanState::set_traits(const FanTraits &traits) {
  this->traits_ = traits;
}
void FanState::set_send_callback(const fan_send_callback_t &send_callback) {
  this->send_callback_ = send_callback;
}
void FanState::set_update_callback(const fan_send_callback_t &update_callback) {
  this->update_callback_ = update_callback;
}

} // namespace fan

} // namespace esphomelib
