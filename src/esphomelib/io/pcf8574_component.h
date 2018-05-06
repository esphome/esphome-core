//
//  pcf8574_component.h
//  esphomelib
//
//  Created by Otto Winter on 05.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_PCF_8574_COMPONENT_H
#define ESPHOMELIB_PCF_8574_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/defines.h"

#ifdef USE_PCF8574

namespace esphomelib {

/// Modes for PCF8574 pins
enum PCF8574GPIOMode {
  PCF8574_INPUT = 0,
  PCF8574_INPUT_PULLUP,
  PCF8574_OUTPUT,
};

namespace io {

class PCF8574GPIOInputPin;
class PCF8574GPIOOutputPin;

class PCF8574Component : public Component {
 public:
  explicit PCF8574Component(uint8_t address, bool pcf8575 = false);

  /** Make a GPIOPin that can be used in other components.
   *
   * Note that in some cases this component might not work with incompatible other integrations
   * because for performance reasons the values are only sent once every loop cycle in a batch.
   * For example, OneWire sensors are not supported.
   *
   * @param pin The pin number to use. 0-7 for PCF8574, 0-15 for PCF8575.
   * @param mode The pin mode to use. Only supported ones are PCF8574_INPUT, PCF8574_INPUT_PULLUP.
   * @param inverted If the pin should invert all incoming and outgoing values.
   * @return An PCF8574GPIOPin instance.
   */
  PCF8574GPIOInputPin make_input_pin(uint8_t pin, uint8_t mode = PCF8574_INPUT, bool inverted = false);

  /** Make a GPIOPin that can be used in other components.
   *
   * Note that in some cases this component might not work with incompatible other integrations
   * because for performance reasons the values are only sent once every loop cycle in a batch.
   * For example, OneWire sensors are not supported.
   *
   * @param pin The pin number to use. 0-7 for PCF8574, 0-15 for PCF8575.
   * @param inverted If the pin should invert all incoming and outgoing values.
   * @return An PCF8574GPIOPin instance.
   */
  PCF8574GPIOOutputPin make_output_pin(uint8_t pin, bool inverted = false);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Check i2c availability and setup masks
  void setup() override;
  /// Write & Read pin masks to the PCF8574
  void loop() override;
  /// Helper function to read the value of a pin. Doesn't do any I/O.
  bool digital_read_(uint8_t pin);
  /// Helper function to write the value of a pin. Doesn't do any I/O.
  void digital_write_(uint8_t pin, bool value);
  /// Helper function to set the pin mode of a pin. Doesn't do any I/O.
  void pin_mode_(uint8_t pin, uint8_t mode);

 protected:
  bool read_gpio_();

  void write_gpio_();

  uint8_t address_;
  uint16_t ddr_mask_{0x00};
  uint16_t input_mask_{0x00};
  uint16_t port_mask_{0x00};
  bool pcf8575_; ///< TRUE->16-channel PCF8575, FALSE->8-channel PCF8574
};

/// Helper class to expose a PCF8574 pin as an internal input GPIO pin.
class PCF8574GPIOInputPin : public GPIOInputPin {
 public:
  PCF8574GPIOInputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() const override;
  bool digital_read() const override;
  void digital_write(bool value) const override;

 protected:
  PCF8574Component *parent_;
};

/// Helper class to expose a PCF8574 pin as an internal output GPIO pin.
class PCF8574GPIOOutputPin : public GPIOOutputPin {
 public:
  PCF8574GPIOOutputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() const override;
  bool digital_read() const override;
  void digital_write(bool value) const override;

 protected:
  PCF8574Component *parent_;
};

} // namespace io

} // namespace esphomelib

#endif //USE_PCF8574

#endif //ESPHOMELIB_PCF_8574_COMPONENT_H
