//
// Created by Otto Winter on 29.12.17.
//

#include "esphomelib/defines.h"

#ifdef USE_FAN

#include "esphomelib/fan/fan_state.h"
#include "esphomelib/esppreferences.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace fan {

static const char *TAG = "fan.state";

bool FanState::get_state() const {
  return this->state_;
}
void FanState::set_state(bool state) {
  this->state_ = state;
  this->state_callback_.call();
}
FanSpeed fan::FanState::get_speed() const {
  return this->speed_;
}
void FanState::set_speed(FanSpeed speed) {
  this->speed_ = speed;
  this->state_callback_.call();
}
bool FanState::is_oscillating() const {
  return this->oscillating_;
}
void FanState::set_oscillating(bool oscillating) {
  this->oscillating_ = oscillating;
  this->state_callback_.call();
}
const FanTraits &FanState::get_traits() const {
  return this->traits_;
}
void FanState::set_traits(const FanTraits &traits) {
  this->traits_ = traits;
}
void FanState::add_on_state_change_callback(std::function<void()> &&update_callback) {
  this->state_callback_.add(std::move(update_callback));
}
FanState::FanState(const std::string &name) : Nameable(name) {}

void FanState::load_from_preferences() {
  this->set_state(global_preferences.get_bool(this->get_name(), "state", false));
  this->set_oscillating(global_preferences.get_bool(this->get_name(), "oscillating", false));
  this->set_speed(static_cast<FanSpeed>(global_preferences.get_int32(this->get_name(), "speed", FAN_SPEED_HIGH)));
}
void FanState::save_to_preferences() {
  global_preferences.put_bool(this->get_name(), "state", this->get_state());
  global_preferences.put_bool(this->get_name(), "oscillating", this->is_oscillating());
  global_preferences.put_int32(this->get_name(), "speed", this->get_speed());
}
bool FanState::set_speed(const char *speed) {
  if (strcasecmp(speed, "off") == 0) {
    ESP_LOGD(TAG, "Turning Fan Speed off.");
    this->set_speed(FAN_SPEED_OFF);
  } else if (strcasecmp(speed, "low") == 0) {
    ESP_LOGD(TAG, "Turning Fan Speed low.");
    this->set_speed(FAN_SPEED_LOW);
  } else if (strcasecmp(speed, "medium") == 0) {
    ESP_LOGD(TAG, "Turning Fan Speed medium.");
    this->set_speed(FAN_SPEED_MEDIUM);
  } else if (strcasecmp(speed, "high") == 0) {
    ESP_LOGD(TAG, "Turning Fan Speed high.");
    this->set_speed(FAN_SPEED_HIGH);
  } else {
    return false;
  }
  return true;
}

} // namespace fan

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAN
