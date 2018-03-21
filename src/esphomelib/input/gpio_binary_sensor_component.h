//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_INPUT_GPIO_BINARY_SENSOR_COMPONENT_H
#define ESPHOMELIB_INPUT_GPIO_BINARY_SENSOR_COMPONENT_H

#include <cstdint>

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

namespace input {

/** GPIOBinarySensorComponent - Simple binary_sensor component for a GPIO pin.
 *
 * This class allows you to observe the digital state of a certain GPIO pin.
 *
 * Example:
 *
 * app.make_simple_gpio_binary_sensor("Window Open", 36, binary_sensor::device_class::WINDOW);
 *
 * or for setting input pinMode:
 *
 * app.make_simple_gpio_binary_sensor("Window Open", GPIOInputPin(36, INPUT_PULLUP),
 *                                    binary_sensor::device_class::WINDOW);
 */
class GPIOBinarySensorComponent : public binary_sensor::BinarySensor, public Component {
 public:
  /** Construct a GPIOBinarySensorComponent.
   *
   * @param pin The input pin, can either be an integer or GPIOInputPin.
   */
  explicit GPIOBinarySensorComponent(GPIOInputPin pin);

  /// Set the pin for this GPIO binary sensor.
  void set_pin(const GPIOInputPin &pin);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  /// Setup pin
  void setup() override;
  /// Hardware priority
  float get_setup_priority() const override;
  /// Check sensor
  void loop() override;

  /// Get the pin for this GPIO binary sensor.
  GPIOInputPin &get_pin();

 protected:
  GPIOInputPin pin_;
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_GPIO_BINARY_SENSOR_COMPONENT_H
