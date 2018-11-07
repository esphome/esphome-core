#include "esphomelib/defines.h"

#ifdef USE_GPIO_SWITCH

#include "esphomelib/switch_/gpio_switch.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.gpio";

GPIOSwitch::GPIOSwitch(const std::string &name, GPIOPin *pin)
    : Switch(name), pin_(pin) {

}

float GPIOSwitch::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
void GPIOSwitch::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO Switch '%s'...", this->get_name().c_str());
  this->pin_->setup();
  bool restored = this->get_initial_state().value_or(false);
  if (restored) {
    this->turn_on();
  } else {
    this->turn_off();
  }
}
void GPIOSwitch::write_state(bool state) {
  this->pin_->digital_write(state);
  this->publish_state(state);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_SWITCH
