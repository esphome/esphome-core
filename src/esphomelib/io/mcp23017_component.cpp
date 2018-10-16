//
//  mcp23017_component.cpp
//  esphomelib
//
//  Created by Lazar Obradovic on 09.10.18.
//  Copyright © 2018 Lazar Obradovic. All rights reserved.
//
// Based on:
//   - https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/
//   - http://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf

#include "esphomelib/defines.h"

#ifdef USE_MCP23017

#include "esphomelib/io/mcp23017_component.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace io {

static const char *TAG = "io.mcp23017";

MCP23017Component::MCP23017Component(I2CComponent *parent, uint8_t address)
    : Component(), I2CDevice(parent, address) {}

void MCP23017Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MCP23017...");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);

  uint8_t iocon;
  if (!this->read_reg_(MCP23017_IOCONA, &iocon)) {
    ESP_LOGE(TAG, "MCP23017 not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  // set defaults, all pins input
 this->write_reg_(MCP23017_IODIRA, 0xFF);
 this->write_reg_(MCP23017_IODIRB, 0xFF);
}

static uint8_t bit_for_pin_(uint8_t pin) {
  return pin % 8;
}
static uint8_t reg_for_pin_(uint8_t pin, uint8_t reg_a, uint8_t reg_b) {
  return (pin < 8) ? reg_a : reg_b;
}

bool MCP23017Component::digital_read_(uint8_t pin) {
  uint8_t bit = bit_for_pin_(pin);
  uint8_t reg_addr = reg_for_pin_(pin, MCP23017_GPIOA, MCP23017_GPIOB);
  uint8_t value;
  this->read_reg_(reg_addr, &value);

  return (value >> bit) & 0x1;
}

void MCP23017Component::digital_write_(uint8_t pin, bool value) {
  uint8_t gpio;
  uint8_t bit = bit_for_pin_(pin);

  uint8_t reg_addr = reg_for_pin_(pin, MCP23017_OLATA, MCP23017_OLATB);
  this->read_reg_(reg_addr, &gpio);

  if (value)
    gpio |= 1 << bit;
  else
    gpio &= ~(1 << bit);

  // write the new GPIO
  reg_addr = reg_for_pin_(pin, MCP23017_GPIOA, MCP23017_GPIOB);
  this->write_reg_(reg_addr, gpio);
}

void MCP23017Component::pin_mode_(uint8_t pin, uint8_t mode) {
  switch (mode) {
    case MCP23017_INPUT:
      this->update_reg_(pin, 1, MCP23017_IODIRA, MCP23017_IODIRB);
      break;
    case MCP23017_INPUT_PULLUP:
      this->update_reg_(pin, 1, MCP23017_IODIRA, MCP23017_IODIRB);
      this->update_reg_(pin, 1, MCP23017_GPPUA, MCP23017_GPPUB);
      break;
    case MCP23017_OUTPUT:
      this->update_reg_(pin, 0, MCP23017_IODIRA, MCP23017_IODIRB);
      break;
    default:
      assert(false);
  }
}

bool MCP23017Component::read_reg_(uint8_t reg, uint8_t *value) {
  if (this->is_failed())
    return false;

  if (!this->read_bytes(reg, value, 1)) {
    this->status_set_warning();
    return false;
  }

  this->status_clear_warning();
  return true;
}

bool MCP23017Component::write_reg_(uint8_t reg, uint8_t value) {
  if (this->is_failed())
    return false;

  if (!this->write_byte(reg, value)) {
    this->status_set_warning();
    return false;
  }

  this->status_clear_warning();
  return true;

}

bool MCP23017Component::update_reg_(uint8_t pin, uint8_t pin_value, uint8_t reg_a, uint8_t reg_b) {
  uint8_t reg_addr = reg_for_pin_(pin, reg_a, reg_a);
  uint8_t bit = bit_for_pin_(pin);

  uint8_t reg_value;
  if (!this->read_reg_(reg_addr, &reg_value)) {
    this->status_set_warning();
    return false;
  }
  // set the value for the particular bit
  if (pin_value)
    reg_value |= 1 << bit;
  else
    reg_value &= ~(1 << bit);

  if(!this->write_reg_(reg_addr, reg_value)) {
    this->status_set_warning();
    return false;
  }
  this->status_clear_warning();
  return true;

}


MCP23017GPIOInputPin MCP23017Component::make_input_pin(uint8_t pin, uint8_t mode, bool inverted) {
  assert(mode == MCP23017_INPUT || mode == MCP23017_INPUT_PULLUP);
  assert(pin < 16);

  return {this, pin, mode, inverted};
}
MCP23017GPIOOutputPin MCP23017Component::make_output_pin(uint8_t pin, bool inverted) {
  assert(pin < 16);
  return {this, pin, MCP23017_OUTPUT, inverted};
}

float MCP23017Component::get_setup_priority() const {
  return setup_priority::HARDWARE;
}






void MCP23017GPIOInputPin::setup() {
  this->pin_mode(this->mode_);
}
bool MCP23017GPIOInputPin::digital_read() {
  return this->parent_->digital_read_(this->pin_) != this->inverted_;
}
void MCP23017GPIOInputPin::digital_write(bool value) {
  this->parent_->digital_write_(this->pin_, value != this->inverted_);
}
MCP23017GPIOInputPin::MCP23017GPIOInputPin(MCP23017Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOInputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *MCP23017GPIOInputPin::copy() const {
  return new MCP23017GPIOInputPin(*this);
}
void MCP23017GPIOInputPin::pin_mode(uint8_t mode) {
  this->parent_->pin_mode_(this->pin_, mode);
}






void MCP23017GPIOOutputPin::setup() {
  this->pin_mode(this->mode_);
}
bool MCP23017GPIOOutputPin::digital_read() {
  return this->parent_->digital_read_(this->pin_) != this->inverted_;
}
void MCP23017GPIOOutputPin::digital_write(bool value) {
  this->parent_->digital_write_(this->pin_, value != this->inverted_);
}
MCP23017GPIOOutputPin::MCP23017GPIOOutputPin(MCP23017Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOOutputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *MCP23017GPIOOutputPin::copy() const {
  return new MCP23017GPIOOutputPin(*this);
}
void MCP23017GPIOOutputPin::pin_mode(uint8_t mode) {
  this->parent_->pin_mode_(this->pin_, mode);
}

} // namespace io

ESPHOMELIB_NAMESPACE_END

#endif //USE_MCP23017
