#ifndef ESPHOME_LCD_DISPLAY_H
#define ESPHOME_LCD_DISPLAY_H

#include "esphome/defines.h"

#ifdef USE_LCD_DISPLAY

#include "esphome/component.h"
#include "esphome/esphal.h"
#include "esphome/i2c_component.h"
#include "esphome/time/rtc_component.h"

ESPHOME_NAMESPACE_BEGIN

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

  /// Print the given text at the specified column and row.
  void print(uint8_t column, uint8_t row, const char *str);
  /// Print the given string at the specified column and row.
  void print(uint8_t column, uint8_t row, const std::string &str);
  /// Print the given text at column=0 and row=0.
  void print(const char *str);
  /// Print the given string at column=0 and row=0.
  void print(const std::string &str);
  /// Evaluate the printf-format and print the text at the specified column and row.
  void printf(uint8_t column, uint8_t row, const char *format, ...) __attribute__((format(printf, 4, 5)));
  /// Evaluate the printf-format and print the text at column=0 and row=0.
  void printf(const char *format, ...) __attribute__((format(printf, 2, 3)));

#ifdef USE_TIME
  /// Evaluate the strftime-format and print the text at the specified column and row.
  void strftime(uint8_t column, uint8_t row, const char *format, time::ESPTime time)
      __attribute__((format(strftime, 4, 0)));
  /// Evaluate the strftime-format and print the text at column=0 and row=0.
  void strftime(const char *format, time::ESPTime time) __attribute__((format(strftime, 2, 0)));
#endif

 protected:
  virtual bool is_four_bit_mode() = 0;
  virtual void write_n_bits(uint8_t value, uint8_t n) = 0;
  virtual void send(uint8_t value, bool rs) = 0;

  void command_(uint8_t value);

  uint8_t columns_;
  uint8_t rows_;
  uint8_t *buffer_{nullptr};
  lcd_writer_t writer_;
};

class GPIOLCDDisplay : public LCDDisplay {
 public:
  GPIOLCDDisplay(uint8_t columns, uint8_t rows, uint32_t update_interval = 1000);
  void setup() override;
  void set_data_pins(const GPIOOutputPin &d0, const GPIOOutputPin &d1, const GPIOOutputPin &d2,
                     const GPIOOutputPin &d3);
  void set_data_pins(const GPIOOutputPin &d0, const GPIOOutputPin &d1, const GPIOOutputPin &d2, const GPIOOutputPin &d3,
                     const GPIOOutputPin &d4, const GPIOOutputPin &d5, const GPIOOutputPin &d6,
                     const GPIOOutputPin &d7);
  void set_enable_pin(const GPIOOutputPin &enable);
  void set_rs_pin(const GPIOOutputPin &rs);
  void set_rw_pin(const GPIOOutputPin &rw);
  void dump_config() override;

 protected:
  bool is_four_bit_mode() override;
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
  PCF8574LCDDisplay(I2CComponent *parent, uint8_t columns, uint8_t rows, uint8_t address = 0x3F,
                    uint32_t update_interval = 1000);
  void setup() override;
  void dump_config() override;

 protected:
  bool is_four_bit_mode() override;
  void write_n_bits(uint8_t value, uint8_t n) override;
  void send(uint8_t value, bool rs) override;
};
#endif

}  // namespace display

ESPHOME_NAMESPACE_END

#endif  // USE_LCD_DISPLAY

#endif  // ESPHOME_LCD_DISPLAY_H
