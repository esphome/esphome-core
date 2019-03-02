#ifndef ESPHOME_ESP_8266_PWM_OUTPUT_H
#define ESPHOME_ESP_8266_PWM_OUTPUT_H

#include "esphome/defines.h"

#ifdef USE_ESP8266_PWM_OUTPUT

#include "esphome/output/float_output.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

/** Software PWM output component for ESP8266.
 *
 * Supported pins are 0-16. By default, this uses a frequency of 1000Hz, and this can only
 * be changed globally for all software PWM pins. To change the PWM frequency, do the following:
 *
 * ```cpp
 * analogWriteFreq(500); // 500Hz frequency for all pins.
 * ```
 *
 * Note that this is a software PWM and can have noticeable flickering because of other
 * interrupts on the ESP8266 (like WiFi). Additionally, this PWM output can only be 80%
 * on at max. That is a known limitation, if it's a deal breaker, consider using the ESP32
 * instead - it up to 16 integrated hardware PWM channels.
 */
class ESP8266PWMOutput : public FloatOutput, public Component {
 public:
  /// Construct the Software PWM output.
  explicit ESP8266PWMOutput(const GPIOOutputPin &pin);

  void set_frequency(float frequency);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  /// Initialize pin
  void setup() override;
  void dump_config() override;
  /// HARDWARE setup_priority
  float get_setup_priority() const override;
  /// Override FloatOutput's write_state for analogWrite

 protected:
  void write_state(float state) override;

  GPIOOutputPin pin_;
  float frequency_{1000.0};
};

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_ESP8266_PWM_OUTPUT

#endif  // ESPHOME_ESP_8266_PWM_OUTPUT_H
