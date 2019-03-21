// Based on:
//   - https://www.onsemi.com/pub/Collateral/CAT9554-D.PDF

#include "esphome/defines.h"

#ifdef USE_CAT9554

#include "esphome/io/cat9554.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace io {

static const char *TAG = "io.cat9554";

static CAT9554Component *instance_;
static void ICACHE_RAM_ATTR HOT gpio_intr(CAT9554Component **instance) {
  (*instance)->update_gpio_needed(true);
}

CAT9554Component::CAT9554Component(I2CComponent *parent, uint8_t address, uint8_t irq)
    : Component(), I2CDevice(parent, address), irq_(irq) {}

void CAT9554Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up CAT9554...");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);
  if (!this->read_gpio_()) {
    ESP_LOGE(TAG, "CAT9554 not available under 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  instance_ = this;
  this->pin_ = new GPIOInputPin(this->irq_);
  this->pin_->setup();
  this->isr_ = this->pin_->to_isr();
  this->pin_->attach_interrupt(gpio_intr, &instance_, FALLING);
  //this->write_gpio_();
  this->read_gpio_();
  this->read_config_();
  this->update_gpio_ = false;
}

void CAT9554Component::dump_config() {
  ESP_LOGCONFIG(TAG, "CAT9554:");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with CAT9554 failed!");
  }
}
bool CAT9554Component::digital_read(uint8_t pin) {
  if (this->update_gpio_) {
    this->read_gpio_();
    this->update_gpio_ = false;
  }
  return this->input_mask_ & (1 << pin);
}
void CAT9554Component::digital_write(uint8_t pin, bool value) {
  if (value) {
    this->output_mask_ |= (1 << pin);
  } else {
    this->output_mask_ &= ~(1 << pin);
  }

  this->write_gpio_();
}
void CAT9554Component::pin_mode(uint8_t pin, uint8_t mode) {
  switch (mode) {
    case CAT9554_INPUT:
      this->config_mask_ |= (1 << pin);
      break;
    case CAT9554_OUTPUT:
      this->config_mask_ &= ~(1 << pin);
      break;
    default:
      break;
  }

  this->config_gpio_();
}
bool CAT9554Component::read_gpio_() {
  if (this->is_failed())
    return false;

    uint8_t data;
    if (!this->parent_->read_byte(this->address_, INPUT_REG & 0xff, &data, 1)) {
      this->status_set_warning();
      return false;
    }
    this->input_mask_ = data;

  this->status_clear_warning();
  return true;
}
bool CAT9554Component::write_gpio_() {
  if (this->is_failed())
    return false;

  if (!this->parent_->write_byte(this->address_, OUTPUT & 0xff, this->output_mask_)) {
    this->status_set_warning();
    return false;
  }

  this->status_clear_warning();
  return true;
}
bool CAT9554Component::config_gpio_() {
  if (this->is_failed())
    return false;

  if (!this->parent_->write_byte(this->address_, INPUT_REG & 0xff, this->config_mask_)) {
    this->status_set_warning();
    return false;
  }
  if (!this->parent_->write_byte(this->address_, CONFIG_REG & 0xff, this->config_mask_)) {
    this->status_set_warning();
    return false;
  }
  if (!this->parent_->write_byte(this->address_, INPUT_REG & 0xff, 0x00)) {
    this->status_set_warning();
    return false;
  }

  this->status_clear_warning();
  return true;
}
bool CAT9554Component::read_config_() {
  if (this->is_failed())
    return false;

    uint8_t data;
    if (!this->parent_->read_byte(this->address_, CONFIG_REG & 0xff, &data, 1)) {
      this->status_set_warning();
      return false;
    }
    this->config_mask_ = data;

  this->status_clear_warning();
  return true;
}
CAT9554GPIOInputPin CAT9554Component::make_input_pin(uint8_t pin, bool inverted) {
  return {this, pin, CAT9554_INPUT, inverted};
}
CAT9554GPIOOutputPin CAT9554Component::make_output_pin(uint8_t pin, bool inverted) {
  return {this, pin, CAT9554_OUTPUT, inverted};
}
float CAT9554Component::get_setup_priority() const { return setup_priority::HARDWARE; }

void CAT9554GPIOInputPin::setup() { this->pin_mode(this->mode_); }
bool CAT9554GPIOInputPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void CAT9554GPIOInputPin::digital_write(bool value) {
  this->parent_->digital_write(this->pin_, value != this->inverted_);
}
CAT9554GPIOInputPin::CAT9554GPIOInputPin(CAT9554Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOInputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *CAT9554GPIOInputPin::copy() const { return new CAT9554GPIOInputPin(*this); }
void CAT9554GPIOInputPin::pin_mode(uint8_t mode) { this->parent_->pin_mode(this->pin_, mode); }

void CAT9554GPIOOutputPin::setup() { this->pin_mode(this->mode_); }
bool CAT9554GPIOOutputPin::digital_read() { return this->parent_->digital_read(this->pin_) != this->inverted_; }
void CAT9554GPIOOutputPin::digital_write(bool value) {
  this->parent_->digital_write(this->pin_, value != this->inverted_);
}
CAT9554GPIOOutputPin::CAT9554GPIOOutputPin(CAT9554Component *parent, uint8_t pin, uint8_t mode, bool inverted)
    : GPIOOutputPin(pin, mode, inverted), parent_(parent) {}
GPIOPin *CAT9554GPIOOutputPin::copy() const { return new CAT9554GPIOOutputPin(*this); }
void CAT9554GPIOOutputPin::pin_mode(uint8_t mode) { this->parent_->pin_mode(this->pin_, mode); }

}  // namespace io

ESPHOME_NAMESPACE_END

#endif  // USE_CAT9554
