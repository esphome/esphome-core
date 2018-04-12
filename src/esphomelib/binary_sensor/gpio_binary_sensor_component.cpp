//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/binary_sensor/gpio_binary_sensor_component.h"

#include "esphomelib/esphal.h"
#include "esphomelib/log.h"

#ifdef USE_GPIO_BINARY_SENSOR

namespace esphomelib {

namespace binary_sensor {

static const char *TAG = "binary_sensor.gpio";

void GPIOBinarySensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO binary sensor.");
  this->pin_.setup();
}

void GPIOBinarySensorComponent::loop() {
  this->publish_state(this->pin_.read_value());
}

float GPIOBinarySensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
GPIOInputPin &GPIOBinarySensorComponent::get_pin() {
  return this->pin_;
}
void GPIOBinarySensorComponent::set_pin(const GPIOInputPin &pin) {
  this->pin_ = pin;
}
GPIOBinarySensorComponent::GPIOBinarySensorComponent(GPIOInputPin pin)
  : pin_(pin) { }

} // namespace binary_sensor

} // namespace esphomelib

#endif //USE_GPIO_BINARY_SENSOR
