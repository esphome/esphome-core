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

/// GPIOBinarySensorComponent - Simple binary_sensor component for a GPIO pin.
class GPIOBinarySensorComponent : public binary_sensor::BinarySensor, public Component {
 public:
  /** Construct a GPIOBinarySensorComponent.
   *
   * @param pin The pin number.
   * @param mode The pin-mode.
   */
  explicit GPIOBinarySensorComponent(uint8_t pin, uint8_t mode = INPUT);

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);

  uint8_t get_mode() const;
  /// Set the pinMode - for example INPUT.
  void set_mode(uint8_t mode);

  void setup() override;
  float get_setup_priority() const override;
  void loop() override;

 protected:
  uint8_t pin_;
  uint8_t mode_;
  bool last_state_;
  bool first_run_;
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_GPIO_BINARY_SENSOR_COMPONENT_H
