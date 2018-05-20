//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/switch_/switch.h"
#include "esphomelib/esppreferences.h"

#ifdef USE_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

std::string Switch::icon() {
  return "";
}
Switch::Switch(const std::string &name) : BinarySensor(name) {
  this->add_on_state_callback([this](bool state) {
    global_preferences.put_bool(this->get_name(), "state", state != this->inverted_);
  });
}

std::string Switch::get_icon() {
  if (this->icon_.has_value())
    return *this->icon_;
  return this->icon();
}

void Switch::set_icon(const std::string &icon) {
  this->icon_ = icon;
}
void Switch::write_state(bool state) {
  if (state != this->inverted_) {
    this->turn_on();
  } else {
    this->turn_off();
  }
}
float Switch::get_setup_priority() const {
  return setup_priority::HARDWARE - 1.0f;
}
void Switch::setup_() {
  this->setup_internal();
  this->setup();

  bool initial_state = global_preferences.get_bool(this->get_name(), "state", false);
  this->write_state(initial_state);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH
