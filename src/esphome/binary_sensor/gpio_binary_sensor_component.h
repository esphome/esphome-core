#ifndef ESPHOME_BINARY_SENSOR_GPIO_BINARY_SENSOR_COMPONENT_H
#define ESPHOME_BINARY_SENSOR_GPIO_BINARY_SENSOR_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_GPIO_BINARY_SENSOR

#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

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
  explicit GPIOBinarySensorComponent(const std::string &name, GPIOPin *pin);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup pin
  void setup() override;
  void dump_config() override;
  /// Hardware priority
  float get_setup_priority() const override;
  /// Check sensor
  void loop() override;

 protected:
  GPIOPin *pin_;
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_GPIO_BINARY_SENSOR

#endif  // ESPHOME_BINARY_SENSOR_GPIO_BINARY_SENSOR_COMPONENT_H
