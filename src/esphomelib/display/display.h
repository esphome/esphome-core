//
//  display.h
//  esphomelib
//
//  Created by Otto Winter on 15.08.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_DISPLAY_H
#define ESPHOMELIB_DISPLAY_H

#include "esphomelib/defines.h"
#include "esphomelib/helpers.h"
#include "esphomelib/time/rtc_component.h"
#include <cstdint>
#include <functional>
#include <vector>
#include <stdarg.h>
#include <cstdio>

#ifdef USE_DISPLAY

ESPHOMELIB_NAMESPACE_BEGIN

// outside of display namespace so that users don't need to write display::TextAlign::TOP_LEFT
enum class TextAlign {
  TOP = 0x00,
  CENTER_VERTICAL = 0x01,
  BASELINE = 0x02,
  BOTTOM = 0x04,

  LEFT = 0x00,
  CENTER_HORIZONTAL = 0x08,
  RIGHT = 0x10,

  TOP_LEFT = TOP | LEFT,
  TOP_CENTER = TOP | CENTER_HORIZONTAL,
  TOP_RIGHT = TOP | RIGHT,

  CENTER_LEFT = CENTER_VERTICAL | LEFT,
  CENTER = CENTER_VERTICAL | CENTER_HORIZONTAL,
  CENTER_RIGHT = CENTER_VERTICAL | RIGHT,

  BASELINE_LEFT = BASELINE | LEFT,
  BASELINE_CENTER = BASELINE | CENTER_HORIZONTAL,
  BASELINE_RIGHT = BASELINE | RIGHT,

  BOTTOM_LEFT = BOTTOM | LEFT,
  BOTTOM_CENTER = BOTTOM | CENTER_HORIZONTAL,
  BOTTOM_RIGHT = BOTTOM | RIGHT,
};

extern const uint8_t COLOR_OFF;
extern const uint8_t COLOR_ON;

namespace display {

enum DisplayRotation {
  DISPLAY_ROTATION_0_DEGREES = 0,
  DISPLAY_ROTATION_90_DEGREES,
  DISPLAY_ROTATION_180_DEGREES,
  DISPLAY_ROTATION_270_DEGREES,
};

class Font;
class Image;
class DisplayBuffer;

using display_writer_t = std::function<void(DisplayBuffer &)>;

class DisplayBuffer {
 public:
  virtual void fill(int color);
  void clear();

  int get_width();
  int get_height();
  void draw_pixel_at(int x, int y, int color = COLOR_ON);

  void line(int x1, int y1, int x2, int y2, int color = COLOR_ON);

  void horizontal_line(int x, int y, int width, int color = COLOR_ON);

  void vertical_line(int x, int y, int height, int color = COLOR_ON);

  void rectangle(int x1, int y1, int width, int height, int color = COLOR_ON);

  void filled_rectangle(int x1, int y1, int width, int height, int color = COLOR_ON);

  void circle(int x_mid, int y_mid, int radius, int color = COLOR_ON);

  void filled_circle(int center_x, int center_y, int radius, int color = COLOR_ON);

  void print(int x, int y, Font *font, int color, TextAlign align, const char *text);

  void print(int x, int y, Font *font, int color, const char *text);

  void print(int x, int y, Font *font, TextAlign align, const char *text);

  void print(int x, int y, Font *font, const char *text);

  void printf(int x, int y, Font *font, int color, TextAlign align, const char *format, ...);

  void printf(int x, int y, Font *font, int color, const char *format, ...);

  void printf(int x, int y, Font *font, TextAlign align, const char *format, ...);

  void printf(int x, int y, Font *font, const char *format, ...);

#ifdef USE_TIME
  void strftime(int x, int y, Font *font, int color, TextAlign align, const char *format, time::EsphomelibTime time);
  void strftime(int x, int y, Font *font, int color, const char *format, time::EsphomelibTime time);
  void strftime(int x, int y, Font *font, TextAlign align, const char *format, time::EsphomelibTime time);
  void strftime(int x, int y, Font *font, const char *format, time::EsphomelibTime time);
#endif

  void image(int x, int y, Image *image);

  void get_text_bounds(int x, int y, const char *text, Font *font, TextAlign align, int *x1, int *y1, int *width, int *height);

  void set_writer(display_writer_t &&writer);

  void set_rotation(DisplayRotation rotation);

 protected:
  void vprintf_(int x, int y, Font *font, int color, TextAlign align, const char *format, va_list arg);

  virtual void draw_absolute_pixel_internal_(int x, int y, int color) = 0;

  virtual int get_height_internal_() = 0;

  virtual int get_width_internal_() = 0;

  void init_internal_(uint32_t buffer_length);

  uint8_t *do_update();

  uint8_t *buffer_{nullptr};
  DisplayRotation rotation_{DISPLAY_ROTATION_0_DEGREES};
  optional<display_writer_t> writer_{};
};

class Glyph {
 public:
  Glyph(const char *aChar, const uint8_t *data_start, uint32_t offset, int offset_x, int offset_y, int width, int height);

  bool get_pixel(int x, int y) const;

  const char *get_char() const;

  bool compare_to(const char *str) const;

  int match_length(const char *str) const;

  void scan_area(int *x1, int *y1, int *width, int *height) const;

 protected:
  friend Font;
  friend DisplayBuffer;

  const char *char_;
  const uint8_t *data_;
  int offset_x;
  int offset_y;
  int width_;
  int height_;
};

class Font {
 public:
  Font(std::vector<Glyph> &&glyphs, int baseline, int bottom);

  int match_next_glyph(const char *str, int *match_length);

  void measure(const char *str, int *width, int *x_offset, int *baseline, int *height);

  const std::vector<Glyph> &get_glyphs() const;

 protected:
  std::vector<Glyph> glyphs_;
  int baseline_;
  int bottom_;
};

class Image {
 public:
  Image(const uint8_t *data_start, int width, int height);
  bool get_pixel(int x, int y) const;
  int get_width() const;
  int get_height() const;

 protected:
  int width_;
  int height_;
  const uint8_t *data_start_;
};

} // namespace display

ESPHOMELIB_NAMESPACE_END

#endif //USE_DISPLAY

#endif //ESPHOMELIB_DISPLAY_H
