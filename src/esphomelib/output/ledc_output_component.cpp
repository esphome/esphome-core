#include "esphomelib/defines.h"

#ifdef USE_LEDC_OUTPUT

#include "esphomelib/output/ledc_output_component.h"

#include <esp32-hal-ledc.h>

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace output {

static const char *TAG = "output.ledc";

void LEDCOutputComponent::write_state(float adjusted_value) {
  uint32_t max_duty = (uint32_t(1) << this->bit_depth_) - 1;
  auto duty = uint32_t(adjusted_value * max_duty);
  ledcWrite(this->channel_, duty);
}

void LEDCOutputComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ledc output...");
  ESP_LOGCONFIG(TAG, "    Pin: %u", this->pin_);
  ESP_LOGCONFIG(TAG, "    LEDC Channel: %u", this->channel_);
  ESP_LOGCONFIG(TAG, "    Frequency: %.1f", this->frequency_);
  ESP_LOGCONFIG(TAG, "    Bit Depth: %u", this->bit_depth_);

  ledcSetup(this->channel_, this->frequency_, this->bit_depth_);
  ledcAttachPin(this->pin_, this->channel_);

  this->turn_off(); // initialize off
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
  this->pin_ = pin;
}
uint8_t LEDCOutputComponent::get_channel() const {
  return this->channel_;
}
void LEDCOutputComponent::set_channel(uint8_t channel) {
  this->channel_ = channel;
}
uint8_t LEDCOutputComponent::get_bit_depth() const {
  return this->bit_depth_;
}
void LEDCOutputComponent::set_bit_depth(uint8_t bit_depth) {
  this->bit_depth_ = bit_depth;
}
float LEDCOutputComponent::get_frequency() const {
  return this->frequency_;
}
void LEDCOutputComponent::set_frequency(float frequency) {
  this->frequency_ = frequency;
}

uint8_t next_ledc_channel = 0;

} // namespace output

ESPHOMELIB_NAMESPACE_END

#endif //USE_LEDC_OUTPUT
