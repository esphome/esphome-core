#include "esphomelib/defines.h"

#ifdef USE_SWITCH

#include "esphomelib/switch_/switch.h"
#include "esphomelib/esppreferences.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

std::string Switch::icon() {
  return "";
}
Switch::Switch(const std::string &name)
  : Nameable(name), state(false) {

}

std::string Switch::get_icon() {
  if (this->icon_.has_value())
    return *this->icon_;
  return this->icon();
}

void Switch::set_icon(const std::string &icon) {
  this->icon_ = icon;
}
void Switch::turn_on() {
  this->write_state(!this->inverted_);
}
void Switch::turn_off() {
  this->write_state(this->inverted_);
}
void Switch::toggle() {
  this->write_state(this->inverted_ == this->state);
}
float Switch::get_setup_priority() const {
  return setup_priority::HARDWARE - 1.0f;
}
void Switch::setup_() {
  this->setup_internal();
  this->setup();

  if (this->do_restore_state()) {
    bool initial_state = global_preferences.get_bool(this->get_name(), "state", this->state);
    auto f = std::bind(&Switch::write_state, this, initial_state);
    this->defer(f);
  }
}
void Switch::publish_state(bool state) {
  this->state = state != this->inverted_;

  global_preferences.put_bool(this->get_name(), "state", this->state);
  this->state_callback_.call(this->state);
}
bool Switch::optimistic() {
  return false;
}

void Switch::add_on_state_callback(std::function<void(bool)> &&callback) {
  this->state_callback_.add(std::move(callback));
}
void Switch::set_inverted(bool inverted) {
  this->inverted_ = inverted;
}
bool Switch::do_restore_state() {
  return true;
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH
