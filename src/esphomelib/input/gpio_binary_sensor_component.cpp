//
// Created by Otto Winter on 26.11.17.
//

#include <esp32-hal-gpio.h>
#include "gpio_binary_sensor_component.h"

namespace esphomelib {

namespace input {

static const char *TAG = "binary_sensor";

void GPIOBinarySensorComponent::setup() {
  ESP_LOGD(TAG, "Setup pin=%d, mode=%d", this->pin_, this->mode_);
  pinMode(this->pin_, this->mode_);
}

void GPIOBinarySensorComponent::loop() {
  auto value = bool(digitalRead(this->pin_));

  if (this->first_run_ || value != this->last_state_) {
    this->first_run_ = false;
    this->last_state_ = value;
    this->publish_state(value);
  }
}

float GPIOBinarySensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

GPIOBinarySensorComponent::GPIOBinarySensorComponent(uint8_t pin, uint8_t mode)
    : BinarySensor(), mode_(mode), first_run_(true) {
  this->set_pin(pin);
}

uint8_t GPIOBinarySensorComponent::get_pin() const {
  return this->pin_;
}
void GPIOBinarySensorComponent::set_pin(uint8_t pin) {
  assert_is_pin(pin);
  assert_construction_state(this);
  this->pin_ = pin;
}
uint8_t GPIOBinarySensorComponent::get_mode() const {
  return this->mode_;
}
void GPIOBinarySensorComponent::set_mode(uint8_t mode) {
  assert_construction_state(this);
  this->mode_ = mode;
}

} // namespace input

} // namespace esphomelib
