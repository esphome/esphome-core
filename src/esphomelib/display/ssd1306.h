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
