//
//  gpio_switch.cpp
//  esphomelib
//
//  Created by Otto Winter on 2018/10/15.
//  Copyright © 2018 Otto Winter. All rights reserved.
//


#include "esphomelib/defines.h"

#ifdef USE_GPIO_SWITCH

#include "esphomelib/switch_/gpio_switch.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.gpio";

GPIOSwitch::GPIOSwitch(const std::string &name, esphomelib::GPIOPin *pin)
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
void GPIOSwitch::turn_on() {
  this->pin_->digital_write(true);
  this->publish_state(true);
}
void GPIOSwitch::turn_off() {
  this->pin_->digital_write(false);
  this->publish_state(true);
}
void GPIOSwitch::set_power_on_value(bool power_on_value) {
  this->power_on_value_ = power_on_value;
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_SWITCH
