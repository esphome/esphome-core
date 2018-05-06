//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/output/gpio_binary_output_component.h"

#include "esphomelib/esphal.h"
#include "esphomelib/log.h"

#ifdef USE_GPIO_OUTPUT

namespace esphomelib {

namespace output {

void GPIOBinaryOutputComponent::write_enabled(bool value) {
  this->pin_->digital_write(value);
}

void GPIOBinaryOutputComponent::setup() {
  this->pin_->setup();
}

float GPIOBinaryOutputComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
GPIOBinaryOutputComponent::GPIOBinaryOutputComponent(GPIOPin *pin)
  : pin_(pin) { }

} // namespace output

} // namespace esphomelib

#endif //USE_GPIO_OUTPUT
