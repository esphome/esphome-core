#include "esphome/defines.h"

#ifdef USE_GPIO_OUTPUT

#include "esphome/output/gpio_binary_output_component.h"

#include "esphome/esphal.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

static const char *TAG = "output.gpio";

void GPIOBinaryOutputComponent::write_state(bool state) { this->pin_->digital_write(state); }

void GPIOBinaryOutputComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO Binary Output...");
  this->turn_off();
  this->pin_->setup();
  this->turn_off();
}
void GPIOBinaryOutputComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "GPIO Binary Output:");
  LOG_PIN("  Pin: ", this->pin_);
  LOG_BINARY_OUTPUT(this);
}

float GPIOBinaryOutputComponent::get_setup_priority() const { return setup_priority::HARDWARE; }
GPIOBinaryOutputComponent::GPIOBinaryOutputComponent(GPIOPin *pin) : pin_(pin) {}

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_GPIO_OUTPUT
