#ifndef ESPHOME_WAVESHARE_E_PAPER_H
#define ESPHOME_WAVESHARE_E_PAPER_H

#include "esphome/defines.h"

#ifdef USE_WAVESHARE_EPAPER

#include "esphome/spi_component.h"
#include "esphome/display/display.h"

ESPHOME_NAMESPACE_BEGIN

namespace display {

class WaveshareEPaper : public PollingComponent, public SPIDevice, public DisplayBuffer {
 public:
  WaveshareEPaper(SPIComponent *parent, GPIOPin *cs, GPIOPin *dc_pin, uint32_t update_interval);
  float get_setup_priority() const override;
  void set_reset_pin(const GPIOOutputPin &reset);
  void set_busy_pin(const GPIOInputPin &busy);

  bool is_device_msb_first() override;
  void command(uint8_t value);
  void data(uint8_t value);

  virtual void display() = 0;

  void update() override;

  void fill(int color) override;

 protected:
  void draw_absolute_pixel_internal(int x, int y, int color) override;

  bool wait_until_idle_();

  void setup_pins_();

  uint32_t get_buffer_length_();

  bool is_device_high_speed() override;

  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();

  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_;
  GPIOPin *busy_pin_{nullptr};
};

enum WaveshareEPaperTypeAModel {
  WAVESHARE_EPAPER_1_54_IN = 0,
  WAVESHARE_EPAPER_2_13_IN,
  WAVESHARE_EPAPER_2_9_IN,
};

class WaveshareEPaperTypeA : public WaveshareEPaper {
 public:
  WaveshareEPaperTypeA(SPIComponent *parent, GPIOPin *cs, GPIOPin *dc_pin, WaveshareEPaperTypeAModel model,
                       uint32_t update_interval);

  void setup() override;

  void dump_config() override;

  void display() override;

  void set_full_update_every(uint32_t full_update_every);

 protected:
  void write_lut_(const uint8_t *lut);

  int get_width_internal() override;

  int get_height_internal() override;

  uint32_t full_update_every_{30};
  uint32_t at_update_{0};
  WaveshareEPaperTypeAModel model_;
};

enum WaveshareEPaperTypeBModel {
  WAVESHARE_EPAPER_2_7_IN = 0,
  WAVESHARE_EPAPER_4_2_IN,
  WAVESHARE_EPAPER_7_5_IN,
};

class WaveshareEPaper2P7In : public WaveshareEPaper {
 public:
  WaveshareEPaper2P7In(SPIComponent *parent, GPIOPin *cs, GPIOPin *dc_pin, uint32_t update_interval);
  void setup() override;

  void display() override;

  void dump_config() override;

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper4P2In : public WaveshareEPaper {
 public:
  WaveshareEPaper4P2In(SPIComponent *parent, GPIOPin *cs, GPIOPin *dc_pin, uint32_t update_interval);
  void setup() override;

  void display() override;

  void dump_config() override;

 protected:
  int get_width_internal() override;

  int get_height_internal() override;

  bool is_device_high_speed() override;
};

class WaveshareEPaper7P5In : public WaveshareEPaper {
 public:
  WaveshareEPaper7P5In(SPIComponent *parent, GPIOPin *cs, GPIOPin *dc_pin, uint32_t update_interval);
  void setup() override;

  void display() override;

  void dump_config() override;

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

}  // namespace display

ESPHOME_NAMESPACE_END

#endif  // USE_WAVESHARE_EPAPER

#endif  // ESPHOME_WAVESHARE_E_PAPER_H
