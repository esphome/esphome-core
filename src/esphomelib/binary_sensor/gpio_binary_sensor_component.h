//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_GPIO_BINARY_SENSOR_COMPONENT_H
#define ESPHOMELIB_BINARY_SENSOR_GPIO_BINARY_SENSOR_COMPONENT_H

#include <cstdint>

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/esphal.h"
#include "esphomelib/defines.h"

#ifdef USE_GPIO_BINARY_SENSOR

namespace esphomelib {

namespace binary_sensor {

/** Simple binary_sensor component for a GPIO pin.
 *
 * This class allows you to observe the digital state of a certain GPIO pin.
 */
class GPIOBinarySensorComponent : public BinarySensor, public Component {
 public:
  /** Construct a GPIOBinarySensorComponent.
   *
   * @param name The name for this binary sensor.
   * @param pin The input pin, can either be an integer or GPIOInputPin.
   */
  explicit GPIOBinarySensorComponent(const std::string &name, GPIOInputPin pin);

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

} // namespace binary_sensor

} // namespace esphomelib

#endif //USE_GPIO_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_GPIO_BINARY_SENSOR_COMPONENT_H
