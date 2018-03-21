//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H
#define ESPHOMELIB_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H

#include "esphomelib/output/binary_output.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

namespace output {

/** GPIOBinaryOutputComponent - Simple binary output component for a GPIO pin.
 *
 * This component allows you to control a GPIO pin as a switch.
 *
 * Example:
 *
 * app.make_gpio_binary_output(33);
 *
 * or for setting input pinMode:
 *
 * app.make_gpio_binary_output(GPIOOutputPin(33, OUTPUT_OPEN_DRAIN));
 *
 * Note that with this output component you actually have two ways of inverting the output:
 * either through the GPIOOutputPin, or the BinaryOutput API. You can use either one of these.
 *
 * This is only an *output component*, not a *switch*, if what you want is a switch, take a look
 * at app.make_gpio_switch();
 */
class GPIOBinaryOutputComponent : public BinaryOutput, public Component {
 public:
  /** Construct the GPIO binary output.
   *
   * @param pin The output pin to use for this output, can be integer or GPIOOutputPin.
   */
  explicit GPIOBinaryOutputComponent(GPIOOutputPin pin);

  /// Manually set the output pin.
  void set_pin(const GPIOOutputPin &pin);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  /// Set pin mode.
  void setup() override;
  /// Hardware setup priority.
  float get_setup_priority() const override;

  /// Return the output pin.
  GPIOOutputPin &get_pin();

  /// Override the BinaryOutput method for writing values to HW.
  void write_enabled(bool value) override;

 protected:
  GPIOOutputPin pin_;
};

} // namespace output

} // namespace esphomelib

#endif //ESPHOMELIB_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H
