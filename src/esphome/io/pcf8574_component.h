#ifndef ESPHOME_PCF_8574_COMPONENT_H
#define ESPHOME_PCF_8574_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_PCF8574

#include "esphome/component.h"
#include "esphome/esphal.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace io {

/// Modes for PCF8574 pins
enum PCF8574GPIOMode {
  PCF8574_INPUT = INPUT,
  PCF8574_INPUT_PULLUP = INPUT_PULLUP,
  PCF8574_OUTPUT = OUTPUT,
};

class PCF8574GPIOInputPin;
class PCF8574GPIOOutputPin;

class PCF8574Component : public Component, public I2CDevice {
 public:
  PCF8574Component(I2CComponent *parent, uint8_t address, bool pcf8575 = false);

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
  /// Helper function to read the value of a pin.
  bool digital_read(uint8_t pin);
  /// Helper function to write the value of a pin.
  void digital_write(uint8_t pin, bool value);
  /// Helper function to set the pin mode of a pin.
  void pin_mode(uint8_t pin, uint8_t mode);

  float get_setup_priority() const override;

  void dump_config() override;

 protected:
  bool read_gpio_();

  bool write_gpio_();

  uint16_t ddr_mask_{0x00};
  uint16_t input_mask_{0x00};
  uint16_t port_mask_{0x00};
  bool pcf8575_;  ///< TRUE->16-channel PCF8575, FALSE->8-channel PCF8574
};

/// Helper class to expose a PCF8574 pin as an internal input GPIO pin.
class PCF8574GPIOInputPin : public GPIOInputPin {
 public:
  PCF8574GPIOInputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() override;
  void pin_mode(uint8_t mode) override;
  bool digital_read() override;
  void digital_write(bool value) override;

 protected:
  PCF8574Component *parent_;
};

/// Helper class to expose a PCF8574 pin as an internal output GPIO pin.
class PCF8574GPIOOutputPin : public GPIOOutputPin {
 public:
  PCF8574GPIOOutputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() override;
  void pin_mode(uint8_t mode) override;
  bool digital_read() override;
  void digital_write(bool value) override;

 protected:
  PCF8574Component *parent_;
};

}  // namespace io

ESPHOME_NAMESPACE_END

#endif  // USE_PCF8574

#endif  // ESPHOME_PCF_8574_COMPONENT_H
