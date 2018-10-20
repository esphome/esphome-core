#include "esphomelib/defines.h"

#ifdef USE_FAN

#include "esphomelib/fan/fan_state.h"
#include "esphomelib/esppreferences.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace fan {

static const char *TAG = "fan.state";

const FanTraits &FanState::get_traits() const {
  return this->traits_;
}
void FanState::set_traits(const FanTraits &traits) {
  this->traits_ = traits;
}
void FanState::add_on_state_callback(std::function<void()> &&callback) {
  this->state_callback_.add(std::move(callback));
}
FanState::FanState(const std::string &name) : Nameable(name) {}

void FanState::load_from_preferences() {
  this->state = global_preferences.get_bool(this->get_name(), "state", false);
  this->oscillating = global_preferences.get_bool(this->get_name(), "oscillating", false);
  this->speed = static_cast<FanSpeed>(global_preferences.get_int32(this->get_name(), "speed", FAN_SPEED_HIGH));
}
void FanState::save_to_preferences() {
  global_preferences.put_bool(this->get_name(), "state", this->state);
  global_preferences.put_bool(this->get_name(), "oscillating", this->oscillating);
  global_preferences.put_int32(this->get_name(), "speed", this->speed);
}
FanState::StateCall FanState::turn_on() {
  return this->make_call().set_state(true);
}
FanState::StateCall FanState::turn_off() {
  return this->make_call().set_state(false);
}
FanState::StateCall FanState::toggle() {
  return this->make_call().set_state(!this->state);
}
FanState::StateCall FanState::make_call() {
  return FanState::StateCall(this);
}

FanState::StateCall::StateCall(FanState *state)
    : state_(state) {

}
FanState::StateCall &set_state(bool state) {
  this->binary_state_ = state;
  return *this;
}
FanState::StateCall &FanState::StateCall::set_oscillating(bool oscillating) {
  this->oscillating_ = oscillating;
  return *this;
}
FanState::StateCall &FanState::StateCall::set_speed(FanSpeed speed) {
  this->speed_ = speed;
  return *this;
}
void FanState::StateCall::perform() {
  if (this->binary_state_.has_value()) {
    this->state_->state = *this->binary_state_;
  }
  if (this->oscillating_.has_value()) {
    this->state_->oscillating = *this->oscillating_;
  }
  if (this->speed_.has_value()) {
    this->state_->speed = *this->speed_;
  }
  this->state_->state_callback_.call();
}
FanState::StateCall &FanState::StateCall::set_speed(const char *speed) {
  if (strcasecmp(speed, "low") == 0) {
    this->set_speed(FAN_SPEED_LOW);
  } else if (strcasecmp(speed, "medium") == 0) {
    this->set_speed(FAN_SPEED_MEDIUM);
  } else if (strcasecmp(speed, "high") == 0) {
    this->set_speed(FAN_SPEED_HIGH);
  }
  return *this;
}

} // namespace fan

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAN
