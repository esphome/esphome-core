#ifndef ESPHOME_SSD_1306_H
#define ESPHOME_SSD_1306_H

#include "esphome/defines.h"

#ifdef USE_SSD1306

#include "esphome/component.h"
#include "esphome/i2c_component.h"
#include "esphome/spi_component.h"
#include "esphome/display/display.h"

ESPHOME_NAMESPACE_BEGIN

namespace display {

enum SSD1306Model {
  SSD1306_MODEL_128_32 = 0,
  SSD1306_MODEL_128_64,
  SSD1306_MODEL_96_16,
  SSD1306_MODEL_64_48,
  SH1106_MODEL_128_32,
  SH1106_MODEL_128_64,
  SH1106_MODEL_96_16,
  SH1106_MODEL_64_48,
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
  void init_reset_();

  bool is_sh1106_() const;

  void draw_absolute_pixel_internal(int x, int y, int color) override;

  int get_height_internal() override;
  int get_width_internal() override;
  size_t get_buffer_length_();
  const char *model_str_();

  SSD1306Model model_{SSD1306_MODEL_128_64};
  GPIOPin *reset_pin_{nullptr};
  bool external_vcc_{false};
};

#ifdef USE_SPI
class SPISSD1306 : public SSD1306, public SPIDevice {
 public:
  SPISSD1306(SPIComponent *parent, GPIOPin *cs, GPIOPin *dc_pin, uint32_t update_interval = 1000);

  void setup() override;

  void dump_config() override;

 protected:
  void command(uint8_t value) override;

  void write_display_data() override;
  bool is_device_msb_first() override;
  bool is_device_high_speed() override;

  GPIOPin *dc_pin_;
};
#endif

#ifdef USE_I2C
class I2CSSD1306 : public SSD1306, public I2CDevice {
 public:
  I2CSSD1306(I2CComponent *parent, uint32_t update_interval = 1000);
  void setup() override;
  void dump_config() override;

 protected:
  void command(uint8_t value) override;
  void write_display_data() override;

  enum ErrorCode { NONE = 0, COMMUNICATION_FAILED } error_code_{NONE};
};
#endif

}  // namespace display

ESPHOME_NAMESPACE_END

#endif  // USE_SSD1306

#endif  // ESPHOME_SSD_1306_H
