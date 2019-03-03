#include "esphome/defines.h"

#ifdef USE_GPIO_BINARY_SENSOR

#include "esphome/binary_sensor/gpio_binary_sensor_component.h"
#include "esphome/esphal.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.gpio";

void GPIOBinarySensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO binary sensor '%s'...", this->name_.c_str());
  this->pin_->setup();
  this->publish_initial_state(this->pin_->digital_read());
}

void GPIOBinarySensorComponent::dump_config() {
  LOG_BINARY_SENSOR("", "GPIO Binary Sensor", this);
  LOG_PIN("  Pin: ", this->pin_);
}

void GPIOBinarySensorComponent::loop() { this->publish_state(this->pin_->digital_read()); }

float GPIOBinarySensorComponent::get_setup_priority() const { return setup_priority::HARDWARE; }
GPIOBinarySensorComponent::GPIOBinarySensorComponent(const std::string &name, GPIOPin *pin)
    : BinarySensor(name), pin_(pin) {}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_GPIO_BINARY_SENSOR
