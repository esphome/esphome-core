#ifndef ESPHOME_DISPLAY_NEXTION_H
#define ESPHOME_DISPLAY_NEXTION_H

#include "esphome/defines.h"

#ifdef USE_NEXTION

#include "esphome/component.h"
#include "esphome/uart_component.h"
#include "esphome/time/rtc_component.h"
#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace display {

class NextionTouchComponent;
class Nextion;

using nextion_writer_t = std::function<void(Nextion &)>;

class Nextion : public PollingComponent, public UARTDevice {
 public:
  void set_component_text(const char *component, const char *text);
  void set_component_text_printf(const char *component, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
  void set_component_value(const char *component, int value);
  void set_component_background_color(const char *component, const char *color);
  void set_component_pressed_background_color(const char *component, const char *color);
  void set_component_font_color(const char *component, const char *color);
  void set_component_pressed_font_color(const char *component, const char *color);
  void set_component_coordinates(const char *component, int x, int y);
  void set_component_font(const char *component, uint8_t font_id);
#ifdef USE_TIME
  void set_nextion_rtc_time(time::ESPTime time);
#endif

  void goto_page(const char *page);
  void hide_component(const char *component);
  void show_component(const char *component);
  void enable_component_touch(const char *component);
  void disable_component_touch(const char *component);
  void add_waveform_data(int component_id, uint8_t channel_number, uint8_t value);
  void display_picture(int picture_id, int x1, int y1);
  void fill_area(int x1, int y1, int width, int height, const char *color);
  void line(int x1, int y1, int x2, int y2, const char *color);
  void rectangle(int x1, int y1, int width, int height, const char *color);
  void circle(int center_x, int center_y, int radius, const char *color);
  void filled_circle(int center_x, int center_y, int radius, const char *color);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  Nextion(UARTComponent *parent, uint32_t update_interval = 5000);
  NextionTouchComponent *make_touch_component(const std::string &name, uint8_t page_id, uint8_t component_id);
  void setup() override;
  float get_setup_priority() const override;
  void update() override;
  void loop() override;
  void set_writer(const nextion_writer_t &writer);

 protected:
  void send_command_(const char *command);
  bool send_command_printf_(const char *format, ...) __attribute__ ((format (printf, 2, 3)));

  bool ack_();
  std::vector<NextionTouchComponent *> touch_;
  optional<nextion_writer_t> writer_;
};

class NextionTouchComponent : public binary_sensor::BinarySensor {
 public:
  NextionTouchComponent(const std::string &name, uint8_t page_id, uint8_t component_id);
  void process(uint8_t page_id, uint8_t component_id, bool on);
 protected:
  uint8_t page_id_;
  uint8_t component_id_;
};

} // namespace display

ESPHOME_NAMESPACE_END

#endif //USE_NEXTION

#endif //ESPHOME_DISPLAY_NEXTION_H
