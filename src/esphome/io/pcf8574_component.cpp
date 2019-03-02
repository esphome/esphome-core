// Based on:
//   - https://github.com/skywodd/pcf8574_arduino_library/
//   - http://www.ti.com/lit/ds/symlink/pcf8574.pdf

#include "esphome/defines.h"

#ifdef USE_PCF8574

#include "esphome/io/pcf8574_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace io {

static const char *TAG = "io.pcf8574";

PCF8574Component::PCF8574Component(I2CComponent *parent, uint8_t address, bool pcf8575)
    : Component(), I2CDevice(parent, address), pcf8575_(pcf8575) {}

void PCF8574Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PCF8574...");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "    Is PCF8575: %s", this->pcf8575_ ? "YES" : "NO");
  if (!this->read_gpio_()) {
    ESP_LOGE(TAG, "PCF8574 not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  this->write_gpio_();
  this->read_gpio_();
}
void PCF8574Component::dump_config() {
  ESP_LOGCONFIG(TAG, "PCF8574:");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "    Is PCF8575: %s", YESNO(this->pcf8575_));
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with PCF8574 failed!");
  }
}
bool PCF8574Component::digital_read(uint8_t pin) {
  this->read_gpio_();
  return this->input_mask_ & (1 << pin);
}
void PCF8574Component::digital_write(uint8_t pin, bool value) {
  if (value) {
    this->port_mask_ |= (1 << pin);
  } else {
    this->port_mask_ &= ~(1 << pin);
  }

  this->write_gpio_();
}
void PCF8574Component::pin_mode(uint8_t pin, uint8_t mode) {
  switch (mode) {
    case PCF8574_INPUT:
      this->ddr_mask_ &= ~(1 << pin);
      this->port_mask_ &= ~(1 << pin);
      break;
    case PCF8574_INPUT_PULLUP:
      this->ddr_mask_ &= ~(1 << pin);
      this->port_mask_ |= (1 << pin);
      break;
    case PCF8574_OUTPUT:
      this->ddr_mask_ |= (1 << pin);
      this->port_mask_ &= ~(1 << pin);
      break;
    default:
      break;
  }

  this->write_gpio_();
}
bool PCF8574Component::read_gpio_() {
  if (this->is_failed())
    return false;

  if (this->pcf8575_) {
    if (!this->parent_->raw_receive_16(this->address_, &this->input_mask_, 1)) {
      this->status_set_warning();
      return false;
    }
  } else {
    uint8_t data;
    if (!this->parent_->raw_receive(this->address_, &data, 1)) {
      this->status_set_warning();
      return false;
    }
    this->input_mask_ = data;
  }

  this->status_clear_warning();
  return true;
}
bool PCF8574Component::write_gpio_() {
  if (this->is_failed())
    return false;

  uint16_t value = (this->input_mask_ & ~this->ddr_mask_) | this->port_mask_;

  this->parent_->raw_begin_transmission(this->address_);
  uint8_t data = value & 0xFF;
  this->parent_->raw_write(this->address_, &data, 1);

  if (this->pcf8575_) {
    data = (value >> 8) & 0xFF;
    this->parent_->raw_write(this->address_, &data, 1);
  }
  if (!this->parent_->raw_end_transmission(this->address_)) {
    this->status_set_warning();
    return false;
  }
  this->status_clear_warning();
  return true;
}
PCF8574GPIOInputPin PCF8574Component::make_input_pin(uint8_t pin, uint8_t mode, bool inverted) {
  return {this, pin, mode, inverted};
}
PCF8574GPIOOutputPin PCF8574Component::make_output_pin(uint8_t pin, bool inverted) {
  return {this, pin, PCF8574_OUTPUT, inverted};
}
float PCF8574Component::get_setup_priority() const { return setup_priority::HARDWARE; }

void PCF8574GPIOInputPin::setup() { this->pin_mode(this->mode_); }
bool PCF8574GPIOInputPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void PCF8574GPIOInputPin::digital_write(bool value) {
  this->parent_->digital_write(this->pin_, value != this->inverted_);
}
PCF8574GPIOInputPin::PCF8574GPIOInputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOInputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *PCF8574GPIOInputPin::copy() const { return new PCF8574GPIOInputPin(*this); }
void PCF8574GPIOInputPin::pin_mode(uint8_t mode) { this->parent_->pin_mode(this->pin_, mode); }

void PCF8574GPIOOutputPin::setup() { this->pin_mode(this->mode_); }
bool PCF8574GPIOOutputPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void PCF8574GPIOOutputPin::digital_write(bool value) {
  this->parent_->digital_write(this->pin_, value != this->inverted_);
}
PCF8574GPIOOutputPin::PCF8574GPIOOutputPin(PCF8574Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOOutputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *PCF8574GPIOOutputPin::copy() const { return new PCF8574GPIOOutputPin(*this); }
void PCF8574GPIOOutputPin::pin_mode(uint8_t mode) { this->parent_->pin_mode(this->pin_, mode); }

}  // namespace io

ESPHOME_NAMESPACE_END

#endif  // USE_PCF8574
