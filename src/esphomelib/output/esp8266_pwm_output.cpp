#include "esphomelib/defines.h"

#ifdef USE_ESP8266_PWM_OUTPUT

#include "esphomelib/output/esp8266_pwm_output.h"

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace output {

ESP8266PWMOutput::ESP8266PWMOutput(const GPIOOutputPin &pin)
    : pin_(pin) {}
void ESP8266PWMOutput::setup() {
  assert(this->pin_.get_pin() <= 16);
  this->pin_.setup();
  this->turn_off();
}
void ESP8266PWMOutput::write_state(float state) {
  const uint16_t max_duty = 1023;
  auto duty = static_cast<uint16_t>(state * max_duty);

  // Also check pin inversion
  if (this->pin_.is_inverted()) {
    duty = max_duty - duty;
  }
  analogWrite(this->pin_.get_pin(), duty);
}
float ESP8266PWMOutput::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

} // namespace output

ESPHOMELIB_NAMESPACE_END

#endif //USE_ESP8266_PWM_OUTPUT
