//
//  lcd_display.h
//  esphomelib
//
//  Created by Otto Winter on 14.08.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_LCD_DISPLAY_H
#define ESPHOMELIB_LCD_DISPLAY_H

#include "esphomelib/defines.h"
#include "esphomelib/esphal.h"
#include "esphomelib/component.h"
#include "esphomelib/io/pcf8574_component.h"
#include "esphomelib/time/rtc_component.h"

#ifdef USE_LCD_DISPLAY

ESPHOMELIB_NAMESPACE_BEGIN

namespace display {

class LCDDisplay;

using lcd_writer_t = std::function<void(LCDDisplay &)>;

class LCDDisplay : public PollingComponent {
 public:
  LCDDisplay(uint8_t columns, uint8_t rows, uint32_t update_interval = 1000);

  void set_writer(lcd_writer_t &&writer);

  void setup() override;
  float get_setup_priority() const override;
  void update() override;
  void display();

  void print(uint8_t column, uint8_t row, const char *str);
  void print(uint8_t column, uint8_t row, const std::string &str);
  void print(const char *str);
  void print(const std::string &str);
  void printf(uint8_t column, uint8_t row, const char *format, ...);
  void printf(const char *format, ...);

#ifdef USE_TIME
  void strftime(uint8_t column, uint8_t row, const char *format, time::EsphomelibTime time);
  void strftime(const char *format, time::EsphomelibTime time);
#endif

 protected:
  virtual bool is_four_bit_mode_() = 0;
  virtual void write_n_bits(uint8_t value, uint8_t n) = 0;
  virtual void send(uint8_t value, bool rs) = 0;

  void command(uint8_t value);

  uint8_t columns_;
  uint8_t rows_;
  uint8_t *buffer_{nullptr};
  lcd_writer_t writer_;
};

class GPIOLCDDisplay : public LCDDisplay {
 public:
  GPIOLCDDisplay(uint8_t columns, uint8_t rows, uint32_t update_interval = 1000);
  void setup() override;
  void set_data_pins(const GPIOOutputPin &d0, const GPIOOutputPin &d1, const GPIOOutputPin &d2, const GPIOOutputPin &d3);
  void set_data_pins(const GPIOOutputPin &d0,
                     const GPIOOutputPin &d1,
                     const GPIOOutputPin &d2,
                     const GPIOOutputPin &d3,
                     const GPIOOutputPin &d4,
                     const GPIOOutputPin &d5,
                     const GPIOOutputPin &d6,
                     const GPIOOutputPin &d7);
  void set_enable_pin(const GPIOOutputPin &enable);
  void set_rs_pin(const GPIOOutputPin &rs);
  void set_rw_pin(const GPIOOutputPin &rw);
 protected:
  bool is_four_bit_mode_() override;
  void write_n_bits(uint8_t value, uint8_t n) override;
  void send(uint8_t value, bool rs) override;
  GPIOPin *rs_pin_{nullptr};
  GPIOPin *rw_pin_{nullptr};
  GPIOPin *enable_pin_{nullptr};
  GPIOPin *data_pins_[8]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
};

#ifdef USE_LCD_DISPLAY_PCF8574
class PCF8574LCDDisplay : public LCDDisplay, public I2CDevice {
 public:
  PCF8574LCDDisplay(I2CComponent *parent, uint8_t columns, uint8_t rows, uint8_t address = 0x3F, uint32_t update_interval = 1000);
  void setup() override;
 protected:
  bool is_four_bit_mode_() override;
  void write_n_bits(uint8_t value, uint8_t n) override;
  void send(uint8_t value, bool rs) override;
};
#endif

} // namespace display

ESPHOMELIB_NAMESPACE_END

#endif //USE_LCD_DISPLAY

#endif //ESPHOMELIB_LCD_DISPLAY_H
