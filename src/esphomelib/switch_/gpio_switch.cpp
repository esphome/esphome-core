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
  ESP_LOGCONFIG(TAG, "Setting up GPIO Switch...");
  this->pin_->setup();
  ESP_LOGCONFIG(TAG, "  Power On Value: %s", this->power_on_value_ ? "ON" : "OFF");
  this->pin_->digital_write(this->power_on_value_);
  this->publish_state(this->power_on_value_);
}
void GPIOSwitch::set_power_on_value(bool power_on_value) {
  this->power_on_value_ = power_on_value;
}
void GPIOSwitch::write_state(bool state) {
  this->pin_->digital_write(state);
  this->publish_state(state);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_SWITCH
