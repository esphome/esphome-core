//
//  spi_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 23.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_SPI

#include "esphomelib/spi_component.h"
#include "esphomelib/log.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "spi";

SPIComponent::SPIComponent(GPIOPin *clk, GPIOPin *miso, GPIOPin *mosi)
    : clk_(clk), miso_(miso), mosi_(mosi){

}

void SPIComponent::write_byte(uint8_t data) {
  uint8_t send_bits = data;
  if (this->msb_first_)
    send_bits = reverse_bits_8(data);

  this->clk_->digital_write(true);
  delayMicroseconds(5);

  for (size_t i = 0; i < 8; i++) {
    delayMicroseconds(5);
    this->clk_->digital_write(false);

    // sampling on leading edge
    this->mosi_->digital_write(send_bits & (1 << i));
    delayMicroseconds(5);
    this->clk_->digital_write(true);
  }

  ESP_LOGVV(TAG, "    Wrote 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)",
            BYTE_TO_BINARY(data), data);
}

uint8_t SPIComponent::read_byte() {
  assert(this->active_cs_ != nullptr);

  this->clk_->digital_write(true);

  uint8_t data = 0;
  for (size_t i = 0; i < 8; i++) {
    delayMicroseconds(5);
    data |= uint8_t(this->miso_->digital_read()) << i;
    this->clk_->digital_write(false);
    delayMicroseconds(5);
    this->clk_->digital_write(true);
  }

  if (this->msb_first_) {
    data = reverse_bits_8(data);
  }

  ESP_LOGVV(TAG, "    Received 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)",
      BYTE_TO_BINARY(data), data);

  return data;
}
void SPIComponent::read_array(uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; i++)
    data[i] = this->read_byte();
}

void SPIComponent::write_array(uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; i++)
    this->write_byte(data[i]);
}

void SPIComponent::enable(GPIOPin *cs, bool msb_first) {
  ESP_LOGVV(TAG, "Enabling SPI Chip on pin %u...", cs->get_pin());
  cs->digital_write(false);

  this->active_cs_ = cs;
  this->msb_first_ = msb_first;
}

void SPIComponent::disable() {
  ESP_LOGVV(TAG, "Disabling SPI Chip on pin %u...", this->active_cs_->get_pin());
  this->active_cs_->digital_write(true);
  this->active_cs_ = nullptr;
}
void SPIComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SPI bus...");
  this->clk_->setup();
  this->clk_->digital_write(true);
  if (this->miso_ != nullptr) {
    this->miso_->setup();
  }
  if (this->mosi_ != nullptr) {
    this->mosi_->setup();
    this->mosi_->digital_write(false);
  }
}
float SPIComponent::get_setup_priority() const {
  return setup_priority::PRE_HARDWARE;
}
void SPIComponent::set_miso(const GPIOInputPin &miso) {
  this->miso_ = miso.copy();
}
void SPIComponent::set_mosi(const GPIOOutputPin &mosi) {
  this->mosi_ = mosi.copy();
}

SPIDevice::SPIDevice(SPIComponent *parent, GPIOPin *cs)
    : parent_(parent), cs_(cs) {}
void SPIDevice::enable() {
  this->parent_->enable(this->cs_, this->msb_first());
}
void SPIDevice::disable() {
  this->parent_->disable();
}
uint8_t SPIDevice::read_byte() {
  return this->parent_->read_byte();
}
void SPIDevice::read_array(uint8_t *data, size_t length) {
  return this->parent_->read_array(data, length);
}
void SPIDevice::write_byte(uint8_t data) {
  return this->parent_->write_byte(data);
}
void SPIDevice::write_array(uint8_t *data, size_t length) {
  this->parent_->write_array(data, length);
}
void SPIDevice::spi_setup() {
  this->cs_->setup();
  this->cs_->digital_write(true);
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_SPI
