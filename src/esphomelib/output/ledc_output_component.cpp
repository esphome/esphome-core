//
// Created by Otto Winter on 26.11.17.
//

#ifdef ARDUINO_ARCH_ESP32

#include "esphomelib/output/ledc_output_component.h"

#include <esp32-hal-ledc.h>

#include "esphomelib/log.h"

namespace esphomelib {

namespace output {

static const char *TAG = "output.ledc";

void LEDCOutputComponent::write_state(float adjusted_value) {
  uint32_t max_duty = (uint32_t(1) << this->bit_depth_) - 1;
  auto duty = uint32_t(adjusted_value * max_duty);
  ledcWrite(this->channel_, duty);
}

void LEDCOutputComponent::setup() {
  ESP_LOGD(TAG, "Setting up LEDCComponent with pin=%u, channel=%u, freq=%f, bit_depth=%u",
           this->pin_, this->channel_, this->frequency_, this->bit_depth_);

  ledcSetup(this->channel_, this->frequency_, this->bit_depth_);
  ledcAttachPin(this->pin_, this->channel_);

  this->disable(); // initialize off
}

float LEDCOutputComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
LEDCOutputComponent::LEDCOutputComponent(uint8_t pin,
                                         float frequency,
                                         uint8_t bit_depth)
    : Component(), FloatOutput() {
  this->set_channel(next_ledc_channel++);
  this->set_frequency(frequency);
  this->set_pin(pin);
  this->set_channel(next_ledc_channel);
  this->set_bit_depth(bit_depth);
}
uint8_t LEDCOutputComponent::get_pin() const {
  return this->pin_;
}
void LEDCOutputComponent::set_pin(uint8_t pin) {
  assert_construction_state(this);
  assert_is_pin(pin);
  this->pin_ = pin;
}
uint8_t LEDCOutputComponent::get_channel() const {
  return this->channel_;
}
void LEDCOutputComponent::set_channel(uint8_t channel) {
  assert_construction_state(this);
  assert(channel < 16);
  this->channel_ = channel;
}
uint8_t LEDCOutputComponent::get_bit_depth() const {
  return this->bit_depth_;
}
void LEDCOutputComponent::set_bit_depth(uint8_t bit_depth) {
  assert_construction_state(this);
  this->bit_depth_ = bit_depth;
}
float LEDCOutputComponent::get_frequency() const {
  return this->frequency_;
}
void LEDCOutputComponent::set_frequency(float frequency) {
  assert_construction_state(this);
  this->frequency_ = frequency;
}

uint8_t next_ledc_channel = 0;

} // namespace output

} // namespace esphomelib

#endif
