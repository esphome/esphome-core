//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/output/gpio_binary_output_component.h"

#include "esphomelib/esphal.h"
#include "esphomelib/log.h"

#ifdef USE_GPIO_OUTPUT

ESPHOMELIB_NAMESPACE_BEGIN

namespace output {

static const char *TAG = "output.gpio";

void GPIOBinaryOutputComponent::write_enabled(bool value) {
  this->pin_->digital_write(value);
}

void GPIOBinaryOutputComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO Binary Output...");
  this->pin_->setup();
  this->pin_->digital_write(this->default_pin_high_);
}

float GPIOBinaryOutputComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
GPIOBinaryOutputComponent::GPIOBinaryOutputComponent(GPIOPin *pin,
                                                     bool default_pin_high)
  : pin_(pin), default_pin_high_(default_pin_high) { }

} // namespace output

ESPHOMELIB_NAMESPACE_END

#endif //USE_GPIO_OUTPUT
