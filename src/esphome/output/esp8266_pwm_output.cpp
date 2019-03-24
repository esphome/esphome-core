#include "esphome/defines.h"

#ifdef USE_ESP8266_PWM_OUTPUT

#ifdef ARDUINO_ESP8266_RELEASE_2_3_0
#error ESP8266 PWM requires at least arduino_core_version 2.4.0
#endif

#include "esphome/output/esp8266_pwm_output.h"
#include "esphome/espmath.h"
#include "esphome/log.h"

#include <core_esp8266_waveform.h>

ESPHOME_NAMESPACE_BEGIN

namespace output {

static const char *TAG = "output.esp8266_pwm";

ESP8266PWMOutput::ESP8266PWMOutput(const GPIOOutputPin &pin) : pin_(pin) {}

void ESP8266PWMOutput::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ESP8266 PWM Output...");
  this->pin_.setup();
  this->turn_off();
}
void ESP8266PWMOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "ESP8266 PWM:");
  LOG_PIN("  Pin: ", &this->pin_);
  ESP_LOGCONFIG(TAG, "  Frequency: %.1f Hz", this->frequency_);
  LOG_FLOAT_OUTPUT(this);
}
void HOT ESP8266PWMOutput::write_state(float state) {
  // Also check pin inversion
  if (this->pin_.is_inverted()) {
    state = 1.0f - state;
  }

  auto total_time_us = static_cast<uint32_t>(roundf(1e6f / this->frequency_));
  auto duty_on = static_cast<uint32_t>(roundf(total_time_us * state));
  uint32_t duty_off = total_time_us - duty_on;

  if (duty_on == 0) {
    stopWaveform(this->pin_.get_pin());
    this->pin_.digital_write(this->pin_.is_inverted());
  } else if (duty_off == 0) {
    stopWaveform(this->pin_.get_pin());
    this->pin_.digital_write(!this->pin_.is_inverted());
  } else {
    startWaveform(this->pin_.get_pin(), duty_on, duty_off, 0);
  }
}
float ESP8266PWMOutput::get_setup_priority() const { return setup_priority::HARDWARE; }
void ESP8266PWMOutput::set_frequency(float frequency) { this->frequency_ = frequency; }

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_ESP8266_PWM_OUTPUT
