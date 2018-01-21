//
// Created by Otto Winter on 02.12.17.
//

#include <esp32-hal-gpio.h>
#include "gpio_binary_output_component.h"

namespace esphomelib {

namespace output {

void GPIOBinaryOutputComponent::write_value(bool value) {
  if (value != this->is_inverted())
    this->enable_atx();

  digitalWrite(this->pin_, uint8_t(value ? HIGH : LOW));
}

GPIOBinaryOutputComponent::GPIOBinaryOutputComponent(uint8_t pin, uint8_t mode)
    : Component(), BinaryOutput(), HighPowerOutput() {
  this->set_pin(pin);
  this->set_mode(mode);
}

void GPIOBinaryOutputComponent::setup() {
  pinMode(this->pin_, this->mode_);
}

float GPIOBinaryOutputComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
uint8_t GPIOBinaryOutputComponent::get_pin() const {
  return this->pin_;
}
void GPIOBinaryOutputComponent::set_pin(uint8_t pin) {
  assert_construction_state(this);
  assert_is_pin(pin);
  this->pin_ = pin;
}
uint8_t GPIOBinaryOutputComponent::get_mode() const {
  return this->mode_;
}
void GPIOBinaryOutputComponent::set_mode(uint8_t mode) {
  assert_construction_state(this);
  this->mode_ = mode;
}

} // namespace output

} // namespace esphomelib
