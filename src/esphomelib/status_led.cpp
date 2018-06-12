//
//  status_led.cpp
//  esphomelib
//
//  Created by Otto Winter on 12.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/status_led.h"
#include "esphomelib/log.h"

#ifdef USE_STATUS_LED

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "status_led";

StatusLEDComponent *global_status_led = nullptr;

StatusLEDComponent::StatusLEDComponent(GPIOPin *pin) : pin_(pin) {
  global_status_led = this;
}
void StatusLEDComponent::setup() {
  ESP_LOGCONFIG("Setting up Status LED...");
  this->pin_->setup();
  this->pin_->digital_write(false);
}
void StatusLEDComponent::loop() {
  if ((global_state & STATUS_LED_ERROR) != 0u) {
    this->pin_->digital_write(millis() % 250u < 150u);
  } else if ((global_state & STATUS_LED_WARNING) != 0u) {
    this->pin_->digital_write(millis() % 1500u < 250u);
  } else {
    this->pin_->digital_write(false);
  }
}
float StatusLEDComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
float StatusLEDComponent::get_loop_priority() const {
  return 50.0f;
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_STATUS_LED
