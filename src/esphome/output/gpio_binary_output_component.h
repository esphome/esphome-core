#ifndef ESPHOME_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H
#define ESPHOME_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_GPIO_OUTPUT

#include "esphome/output/binary_output.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

/** Simple binary output component for a GPIO pin.
 *
 * This component allows you to control a GPIO pin as a switch.
 *
 * Note that with this output component you actually have two ways of inverting the output:
 * either through the GPIOOutputPin, or the BinaryOutput API. You can use either one of these.
 *
 * This is only an *output component*, not a *switch*, if what you want is a switch, take a look
 * at App.make_gpio_switch();
 */
class GPIOBinaryOutputComponent : public BinaryOutput, public Component {
 public:
  /** Construct the GPIO binary output.
   *
   * @param pin The output pin to use for this output, can be integer or GPIOOutputPin.
   */
  explicit GPIOBinaryOutputComponent(GPIOPin *pin);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  /// Set pin mode.
  void setup() override;
  void dump_config() override;
  /// Hardware setup priority.
  float get_setup_priority() const override;

 protected:
  /// Override the BinaryOutput method for writing values to HW.
  void write_state(bool state) override;

  GPIOPin *pin_;
};

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_GPIO_OUTPUT

#endif  // ESPHOME_OUTPUT_GPIO_BINARY_OUTPUT_COMPONENT_H
