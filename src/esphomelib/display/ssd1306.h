//
//  ssd1306.h
//  esphomelib
//
//  Created by Otto Winter on 15.08.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SSD_1306_H
#define ESPHOMELIB_SSD_1306_H

#include "esphomelib/defines.h"
#include "esphomelib/component.h"
#include "esphomelib/i2c_component.h"
#include "esphomelib/spi_component.h"
#include "esphomelib/display/display.h"

#ifdef USE_SSD1306

ESPHOMELIB_NAMESPACE_BEGIN

namespace display {

static const uint8_t SSD1306_COMMAND_DISPLAY_OFF = 0xAF;
static const uint8_t SSD1306_COMMAND_DISPLAY_ON = 0xAE;
static const uint8_t SSD1306_COMMAND_SET_DISPLAY_CLOCK_DIV = 0xD5;
static const uint8_t SSD1306_COMMAND_SET_MULTIPLEX = 0xA8;
static const uint8_t SSD1306_COMMAND_SET_DISPLAY_OFFSET = 0xD3;
static const uint8_t SSD1306_COMMAND_SET_START_LINE = 0x40;
static const uint8_t SSD1306_COMMAND_CHARGE_PUMP = 0x8D;
static const uint8_t SSD1306_COMMAND_MEMORY_MODE = 0x20;
static const uint8_t SSD1306_COMMAND_SEGRE_MAP = 0xA0;
static const uint8_t SSD1306_COMMAND_COM_SCAN_INC = 0xC0;
static const uint8_t SSD1306_COMMAND_COM_SCAN_DEC = 0xC8;
static const uint8_t SSD1306_COMMAND_SET_COM_PINS = 0xDA;
static const uint8_t SSD1306_COMMAND_SET_CONTRAST = 0x81;
static const uint8_t SSD1306_COMMAND_SET_PRE_CHARGE = 0xD9;
static const uint8_t SSD1306_COMMAND_SET_VCOM_DETECT = 0xDB;
static const uint8_t SSD1306_COMMAND_DISPLAY_ALL_ON_RESUME = 0xA4;
static const uint8_t SSD1306_COMMAND_DEACTIVATE_SCROLL = 0x2E;
static const uint8_t SSD1306_COMMAND_COLUMN_ADDRESS = 0x21;
static const uint8_t SSD1306_COMMAND_PAGE_ADDRESS = 0x22;

static const uint8_t SSD1306_NORMAL_DISPLAY = 0xA6;

enum SSD1306Model {
  SSD1306_MODEL_128_32 = 0,
  SSD1306_MODEL_128_64,
  SSD1306_MODEL_96_16,
  SH1106_MODEL_128_32,
  SH1106_MODEL_128_64,
  SH1106_MODEL_96_16,
};

class SSD1306 : public PollingComponent, public DisplayBuffer {
 public:
  SSD1306(uint32_t update_interval = 1000);
  void setup() override;

  void display();

  void update() override;

  void set_model(SSD1306Model model);
  void set_reset_pin(const GPIOOutputPin &reset_pin);
  void set_external_vcc(bool external_vcc);

  float get_setup_priority() const override;
  void fill(int color) override;

 protected:
  virtual void command(uint8_t value) = 0;
  virtual void write_display_data() = 0;

  bool is_sh1106_() const;

  void draw_absolute_pixel_internal_(int x, int y, int color) override;

  int get_height_internal_() override;
  int get_width_internal_() override;
  size_t get_buffer_length_();

  SSD1306Model model_{SSD1306_MODEL_128_64};
  GPIOPin *reset_pin_{nullptr};
  bool external_vcc_{false};
};

#ifdef USE_SPI
class SPISSD1306 : public SSD1306, public SPIDevice {
 public:
  SPISSD1306(SPIComponent *parent, GPIOPin *cs, GPIOPin *dc_pin, uint32_t update_interval = 1000);

  void setup() override;

 protected:
  void command(uint8_t value) override;

  void write_display_data() override;
  bool msb_first() override;

  GPIOPin *dc_pin_;
};
#endif

#ifdef USE_I2C
class I2CSSD1306 : public SSD1306, public I2CDevice {
 public:
  I2CSSD1306(I2CComponent *parent, uint32_t update_interval = 1000);
  void setup() override;
 protected:
  void command(uint8_t value) override;
  void write_display_data() override;
};
#endif

} // namespace display

ESPHOMELIB_NAMESPACE_END

#endif //USE_SSD1306

#endif //ESPHOMELIB_SSD_1306_H
