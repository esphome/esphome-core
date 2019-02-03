#include "esphomelib/defines.h"

#ifdef USE_GPIO_SWITCH

#include "esphomelib/switch_/gpio_switch.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.gpio";

GPIOSwitch::GPIOSwitch(const std::string &name, GPIOPin *pin)
    : Switch(name), Component(), pin_(pin) {

}

float GPIOSwitch::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
void GPIOSwitch::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO Switch '%s'...", this->name_.c_str());

  bool initial_state = false;
  switch (this->restore_mode_) {
    case GPIO_SWITCH_RESTORE_DEFAULT_OFF:
      initial_state = this->get_initial_state().value_or(false);
      break;
    case GPIO_SWITCH_RESTORE_DEFAULT_ON:
      initial_state = this->get_initial_state().value_or(true);
      break;
    case GPIO_SWITCH_ALWAYS_OFF:
      initial_state = false;
      break;
    case GPIO_SWITCH_ALWAYS_ON:
      initial_state = true;
      break;
  }

  // write state before setup
  this->pin_->digital_write(initial_state != this->inverted_);
  this->pin_->setup();
  // write after setup again for other IOs
  this->pin_->digital_write(initial_state != this->inverted_);
  this->publish_state(initial_state != this->inverted_);
}
void GPIOSwitch::dump_config() {
  LOG_SWITCH("", "GPIO Switch", this);
  LOG_PIN("  Pin: ", this->pin_);
  const char *restore_mode = "";
  switch (this->restore_mode_) {
    case GPIO_SWITCH_RESTORE_DEFAULT_OFF:
      restore_mode = "Restore (Default to OFF)";
      break;
    case GPIO_SWITCH_RESTORE_DEFAULT_ON:
      restore_mode = "Restore (Default to ON)";
      break;
    case GPIO_SWITCH_ALWAYS_OFF:
      restore_mode = "Always OFF";
      break;
    case GPIO_SWITCH_ALWAYS_ON:
      restore_mode = "Always ON";
      break;
  }
  ESP_LOGCONFIG(TAG, "  Restore Mode: %s", restore_mode);
}
void GPIOSwitch::write_state(bool state) {
  this->pin_->digital_write(state);
  this->publish_state(state);
}
void GPIOSwitch::set_restore_mode(GPIOSwitchRestoreMode restore_mode) {
  this->restore_mode_ = restore_mode;
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_SWITCH
