//
//  esp8266_pwm_output.h
//  esphomelib
//
//  Created by Otto Winter on 25.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_ESP_8266_PWM_OUTPUT_H
#define ESPHOMELIB_ESP_8266_PWM_OUTPUT_H

#ifdef ARDUINO_ARCH_ESP8266

#include "esphomelib/output/float_output.h"

namespace esphomelib {

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
  /** Construct the Software PWM output.
   *
   * @param pin The pin to be used (inversion of the pin will be ignored, use the direct set_inverted()) method instead.
   */
  explicit ESP8266PWMOutput(const GPIOOutputPin &pin);

  /// Get the output pin used by this component.
  GPIOOutputPin &get_pin();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void set_pin(const GPIOOutputPin &pin);

  /// Initialize pin
  void setup() override;
  /// HARDWARE setup_priority
  float get_setup_priority() const override;
  /// Override FloatOutput's write_state for analogWrite
  void write_state(float state) override;

 protected:
  GPIOOutputPin pin_;

};

} // namespace output

} // namespace esphomelib

#endif //ARDUINO_ARCH_ESP8266

#endif //ESPHOMELIB_ESP_8266_PWM_OUTPUT_H
