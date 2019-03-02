#include "esphome/defines.h"

#ifdef USE_SPI

#include "esphome/spi_component.h"
#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

static const char *TAG = "spi";

SPIComponent::SPIComponent(GPIOPin *clk, GPIOPin *miso, GPIOPin *mosi) : clk_(clk), miso_(miso), mosi_(mosi) {}

void ICACHE_RAM_ATTR HOT SPIComponent::write_byte(uint8_t data) {
  uint8_t send_bits = data;
  if (this->msb_first_)
    send_bits = reverse_bits_8(data);

  this->clk_->digital_write(true);
  if (!this->high_speed_)
    delayMicroseconds(5);

  for (size_t i = 0; i < 8; i++) {
    if (!this->high_speed_)
      delayMicroseconds(5);
    this->clk_->digital_write(false);

    // sampling on leading edge
    this->mosi_->digital_write(send_bits & (1 << i));
    if (!this->high_speed_)
      delayMicroseconds(5);
    this->clk_->digital_write(true);
  }

  ESP_LOGVV(TAG, "    Wrote 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)", BYTE_TO_BINARY(data), data);
}

uint8_t ICACHE_RAM_ATTR HOT SPIComponent::read_byte() {
  this->clk_->digital_write(true);

  uint8_t data = 0;
  for (size_t i = 0; i < 8; i++) {
    if (!this->high_speed_)
      delayMicroseconds(5);
    data |= uint8_t(this->miso_->digital_read()) << i;
    this->clk_->digital_write(false);
    if (!this->high_speed_)
      delayMicroseconds(5);
    this->clk_->digital_write(true);
  }

  if (this->msb_first_) {
    data = reverse_bits_8(data);
  }

  ESP_LOGVV(TAG, "    Received 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)", BYTE_TO_BINARY(data), data);

  return data;
}
void ICACHE_RAM_ATTR HOT SPIComponent::read_array(uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; i++)
    data[i] = this->read_byte();
}

void ICACHE_RAM_ATTR HOT SPIComponent::write_array(uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; i++) {
    feed_wdt();
    this->write_byte(data[i]);
  }
}

void ICACHE_RAM_ATTR HOT SPIComponent::enable(GPIOPin *cs, bool msb_first, bool high_speed) {
  ESP_LOGVV(TAG, "Enabling SPI Chip on pin %u...", cs->get_pin());
  cs->digital_write(false);

  this->active_cs_ = cs;
  this->msb_first_ = msb_first;
  this->high_speed_ = high_speed;
}

void ICACHE_RAM_ATTR HOT SPIComponent::disable() {
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
void SPIComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "SPI bus:");
  LOG_PIN("  CLK Pin: ", this->clk_);
  LOG_PIN("  MISO Pin: ", this->miso_);
  LOG_PIN("  MOSI Pin: ", this->mosi_);
}
float SPIComponent::get_setup_priority() const { return setup_priority::PRE_HARDWARE; }
void SPIComponent::set_miso(const GPIOInputPin &miso) { this->miso_ = miso.copy(); }
void SPIComponent::set_mosi(const GPIOOutputPin &mosi) { this->mosi_ = mosi.copy(); }

SPIDevice::SPIDevice(SPIComponent *parent, GPIOPin *cs) : parent_(parent), cs_(cs) {}
void HOT SPIDevice::enable() {
  this->parent_->enable(this->cs_, this->is_device_msb_first(), this->is_device_high_speed());
}
void HOT SPIDevice::disable() { this->parent_->disable(); }
uint8_t HOT SPIDevice::read_byte() { return this->parent_->read_byte(); }
void HOT SPIDevice::read_array(uint8_t *data, size_t length) { return this->parent_->read_array(data, length); }
void HOT SPIDevice::write_byte(uint8_t data) { return this->parent_->write_byte(data); }
void HOT SPIDevice::write_array(uint8_t *data, size_t length) { this->parent_->write_array(data, length); }
void SPIDevice::spi_setup() {
  this->cs_->setup();
  this->cs_->digital_write(true);
}
bool HOT SPIDevice::is_device_high_speed() { return false; }

ESPHOME_NAMESPACE_END

#endif  // USE_SPI
