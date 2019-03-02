#ifndef ESPHOME_SPI_COMPONENT_H
#define ESPHOME_SPI_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_SPI

#include "esphome/component.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

class SPIComponent : public Component {
 public:
  SPIComponent(GPIOPin *clk, GPIOPin *miso, GPIOPin *mosi);

  void setup() override;

  void dump_config() override;

  uint8_t read_byte();

  void read_array(uint8_t *data, size_t length);

  void write_byte(uint8_t data);

  void write_array(uint8_t *data, size_t length);

  void enable(GPIOPin *cs, bool msb_first, bool high_speed);

  void disable();

  float get_setup_priority() const override;

  void set_miso(const GPIOInputPin &miso);

  void set_mosi(const GPIOOutputPin &mosi);

 protected:
  GPIOPin *clk_;
  GPIOPin *miso_;
  GPIOPin *mosi_;
  GPIOPin *active_cs_{nullptr};
  bool msb_first_{true};
  bool high_speed_{false};
};

class SPIDevice {
 public:
  SPIDevice(SPIComponent *parent, GPIOPin *cs);

  void spi_setup();

  void enable();

  void disable();

  uint8_t read_byte();

  void read_array(uint8_t *data, size_t length);

  void write_byte(uint8_t data);

  void write_array(uint8_t *data, size_t length);

 protected:
  virtual bool is_device_msb_first() = 0;

  virtual bool is_device_high_speed();

  SPIComponent *parent_;
  GPIOPin *cs_;
};

ESPHOME_NAMESPACE_END

#endif  // USE_SPI

#endif  // ESPHOME_SPI_COMPONENT_H
