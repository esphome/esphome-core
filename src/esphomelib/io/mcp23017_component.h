//
//  mcp23017_component.h
//  esphomelib
//
//  Created by Lazar Obradovic on 09.10.18.
//  Copyright © 2018 Lazar Obradovic. All rights reserved.
//

#ifndef ESPHOMELIB_MCP_23017_COMPONENT_H
#define ESPHOMELIB_MCP_23017_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_MCP23017

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/i2c_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

/// Modes for MCP23017 pins
enum MCP23017GPIOMode {
  MCP23017_INPUT = INPUT,   // 0x00
  MCP23017_INPUT_PULLUP = INPUT_PULLUP, // 0x02
  MCP23017_OUTPUT = OUTPUT, // 0x01
};

enum MCP23017GPIORegisters {
    // A side
    MCP23017_IODIRA = 0x00,
    MCP23017_IPOLA = 0x02,
    MCP23017_GPINTENA = 0x04,
    MCP23017_DEFVALA = 0x06,
    MCP23017_INTCONA = 0x08,
    MCP23017_IOCONA = 0x0A,
    MCP23017_GPPUA = 0x0C,
    MCP23017_INTFA = 0x0E,
    MCP23017_INTCAPA = 0x10,
    MCP23017_GPIOA = 0x12,
    MCP23017_OLATA = 0x14,
    // B side
    MCP23017_IODIRB = 0x01,
    MCP23017_IPOLB = 0x03,
    MCP23017_GPINTENB = 0x05,
    MCP23017_DEFVALB = 0x07,
    MCP23017_INTCONB = 0x09,
    MCP23017_IOCONB = 0x0B,
    MCP23017_GPPUB = 0x0D,
    MCP23017_INTFB = 0x0F,
    MCP23017_INTCAPB = 0x11,
    MCP23017_GPIOB = 0x13,
    MCP23017_OLATB = 0x15,

};

namespace io {

class MCP23017GPIOInputPin;
class MCP23017GPIOOutputPin;

class MCP23017Component : public Component, public I2CDevice {
 public:
  MCP23017Component(I2CComponent *parent, uint8_t address);

  /** Make a GPIOPin that can be used in other components.
   *
   * Note that in some cases this component might not work with incompatible other integrations
   * because for performance reasons the values are only sent once every loop cycle in a batch.
   * For example, OneWire sensors are not supported.
   *
   * @param pin The pin number to use. 0-15 for MCP23017.
   * @param mode The pin mode to use. Only supported ones are MCP23017_INPUT, MCP23017_INPUT_PULLUP.
   * @param inverted If the pin should invert all incoming and outgoing values.
   * @return An MCP23017PIOPin instance.
   */
  MCP23017GPIOInputPin make_input_pin(uint8_t pin, uint8_t mode = MCP23017_INPUT, bool inverted = false);

  /** Make a GPIOPin that can be used in other components.
   *
   * Note that in some cases this component might not work with incompatible other integrations
   * because for performance reasons the values are only sent once every loop cycle in a batch.
   * For example, OneWire sensors are not supported.
   *
   * @param pin The pin number to use. 0-15 for MCP23017.
   * @param inverted If the pin should invert all incoming and outgoing values.
   * @return An MCP23017GPIOPin instance.
   */
  MCP23017GPIOOutputPin make_output_pin(uint8_t pin, bool inverted = false);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Check i2c availability and setup masks
  void setup() override;
  /// Helper function to read the value of a pin.
  bool digital_read_(uint8_t pin);
  /// Helper function to write the value of a pin.
  void digital_write_(uint8_t pin, bool value);
  /// Helper function to set the pin mode of a pin.
  void pin_mode_(uint8_t pin, uint8_t mode);

  float get_setup_priority() const override;

 protected:
  // figure out which bit is it, for a given pin number.
  // problem comes from A/B side mapping, and reverse order of high-low byte value
  uint8_t bit_for_pin_(uint8_t pin);
  // figure out which register of the given 2 is it.
  uint8_t reg_for_pin_ (uint8_t pin, uint8_t regA, uint8_t regB);

  // read a given register
  bool read_reg_ (uint8_t reg, uint8_t *value);
  // write a value to a given register
  bool write_reg_ (uint8_t reg, uint8_t value);
  // update registers with given pin value.
  bool update_reg_ (uint8_t pin, uint8_t pin_value, uint8_t regA, uint8_t regB);

};

/// Helper class to expose a MCP23017 pin as an internal input GPIO pin.
class MCP23017GPIOInputPin : public GPIOInputPin {
 public:
  MCP23017GPIOInputPin(MCP23017Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() override;
  void pin_mode(uint8_t mode) override;
  bool digital_read() override;
  void digital_write(bool value) override;

 protected:
  MCP23017Component *parent_;
};

/// Helper class to expose a MCP23017 pin as an internal output GPIO pin.
class MCP23017GPIOOutputPin : public GPIOOutputPin {
 public:
  MCP23017GPIOOutputPin(MCP23017Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() override;
  void pin_mode(uint8_t mode) override;
  bool digital_read() override;
  void digital_write(bool value) override;

 protected:
  MCP23017Component *parent_;
};

} // namespace io

ESPHOMELIB_NAMESPACE_END

#endif //USE_MCP23017

#endif //ESPHOMELIB_MCP_23017_COMPONENT_H
