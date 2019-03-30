#include "esphome/defines.h"

#ifdef USE_DISPLAY

#include "esphome/display/display.h"
#include "esphome/log.h"
#include "esphome/espmath.h"

#include <pgmspace.h>

ESPHOME_NAMESPACE_BEGIN

namespace display {

static const char *TAG = "display.display";

Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : red(red), green(green), blue(blue), alpha(alpha) {}

const Color Color::WHITE = Color::rgb(0xffffff);
const Color Color::LIGHT_GRAY = Color::rgb(0xbfbfbf);
const Color Color::GRAY = Color::rgb(0x7f7f7f);
const Color Color::DARK_GRAY = Color::rgb(0x3f3f3f);
const Color Color::BLACK = Color::rgb(0x000000);
const Color Color::CLEAR = Color::rgba(0x00000000);
const Color Color::BLUE = Color::rgb(0x0000ff);
const Color Color::NAVY = Color::rgb(0x00007f);
const Color Color::ROYAL = Color::rgb(0x4169e1);
const Color Color::SLATE = Color::rgb(0x708090);
const Color Color::SKY = Color::rgb(0x87ceeb);
const Color Color::CYAN = Color::rgb(0x00ffff);
const Color Color::TEAL = Color::rgb(0x007f7f);
const Color Color::GREEN = Color::rgb(0x00ff00);
const Color Color::CHARTREUSE = Color::rgb(0x7fff00);
const Color Color::LIME = Color::rgb(0x32cd32);
const Color Color::FOREST = Color::rgb(0x228b22);
const Color Color::OLIVE = Color::rgb(0x6b8e23);
const Color Color::YELLOW = Color::rgb(0xffff00);
const Color Color::GOLD = Color::rgb(0xffd700);
const Color Color::GOLDENROD = Color::rgb(0xdaa520);
const Color Color::ORANGE = Color::rgb(0xffa500);
const Color Color::BROWN = Color::rgb(0x8b4513);
const Color Color::TAN = Color::rgb(0xd2b48c);
const Color Color::FIREBRICK = Color::rgb(0xb22222);
const Color Color::RED = Color::rgb(0xff0000);
const Color Color::SCARLET = Color::rgb(0xff341c);
const Color Color::CORAL = Color::rgb(0xff7f50);
const Color Color::SALMON = Color::rgb(0xfa8072);
const Color Color::PINK = Color::rgb(0xff69b4);
const Color Color::MAGENTA = Color::rgb(0xff00ff);
const Color Color::PURPLE = Color::rgb(0xa020f0);
const Color Color::VIOLET = Color::rgb(0xee82ee);
const Color Color::MAROON = Color::rgb(0xb03060);

Color Color::rgb(uint8_t red, uint8_t green, uint8_t blue) {
  return Color(red, green, blue, 0xFF);
}
Color Color::rgb_float(float red, float green, float blue) {
  red = clamp(0.0f, 1.0f, red);
  green = clamp(0.0f, 1.0f, green);
  blue = clamp(0.0f, 1.0f, blue);
  return Color(red * 0xFF, green * 0xFF, blue * 0xFF, 0xFF);
}
Color Color::rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
  return Color(red, green, blue, alpha);
}
Color Color::rgba_float(float red, float green, float blue, float alpha) {
  red = clamp(0.0f, 1.0f, red);
  green = clamp(0.0f, 1.0f, green);
  blue = clamp(0.0f, 1.0f, blue);
  alpha = clamp(0.0f, 1.0f, alpha);
  return Color(red * 0xFF, green * 0xFF, blue * 0xFF, alpha * 0xFF);
}
Color Color::rgb(uint32_t rgb) {
  return Color(rgb >> 16, rgb >> 8, rgb >> 0, 0xFF);
}
Color Color::rgba(uint32_t rgba) {
  return Color(rgba >> 24, rgba >> 16, rgba >> 8, rgba >> 0);
}
Color Color::argb(uint32_t argb) {
  return Color(argb >> 16, argb >> 8, argb >> 0, argb >> 24);
}
Color Color::brightness(uint8_t bright) {
  return Color(bright, bright, bright, 0xFF);
}
Color Color::brightness_float(float bright) {
  bright = clamp(0.0f, 1.0f, bright);
  return Color(bright * 0xFF, bright * 0xFF, bright * 0xFF, 0xFF);
}
Color Color::parse_hex(const std::string &str) {
  int offset = 0;
  if (str.find('#') == 0) offset = 1;
  int len = str.size() - offset;
  int r = 0x00;
  int g = 0x00;
  int b = 0x00;
  int a = 0xFF;
  const char *s = str.c_str() + offset;
  if (len == 3) {
    // single char rgb, like #aaa
    sscanf(s, "%01x%01x%01x", &r, &g, &b);
    r *= 17;
    g *= 17;
    b *= 17;
  } else if (len == 4) {
    // single char rgba, like #aaaf
    sscanf(s, "%01x%01x%01x%01x", &r, &g, &b, &a);
    r *= 17;
    g *= 17;
    b *= 17;
    a *= 17;
  } else if (len == 6) {
    // double char rgb, like #efefef
    sscanf(s, "%02x%02x%02x", &r, &g, &b);
  } else if (len == 8) {
    // double char rgba, like #efefefff
    sscanf(s, "%02x%02x%02x%02x", &r, &g, &b, &a);
  }
  return Color(r, g, b, a);
}
Color Color::from_hsv_float(float h, float s, float v) {
  h = clamp(0.0f, 360.0f, h);
  s = clamp(0.0f, 1.0f, s);
  v = clamp(0.0f, 1.0f, v);

  float x = fmodf(h / 60.0f, 6.0f);
  int i = static_cast<int>(x);
  float f = x - i;
  float p = v * (1 - s);
  float q = v * (1 - s * f);
  float t = v * (1 - s * (1 - f));
  float r, g, b;
  switch (i) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    default:
      r = v;
      g = p;
      b = q;
  }
  return Color::rgb_float(r, g, b);
}
Color Color::lerp(Color initial, Color target, float t) {
  t = clamp(0.0f, 1.0f, t);
  uint8_t r = roundf(esphome::lerp(initial.r, target.r, t));
  uint8_t g = roundf(esphome::lerp(initial.g, target.g, t));
  uint8_t b = roundf(esphome::lerp(initial.b, target.b, t));
  uint8_t a = roundf(esphome::lerp(initial.a, target.a, t));
  return Color(r, g, b, a);
}
bool Color::is_black() const {
  return this->red == 0 && this->green == 0 && this->blue == 0;
}
Color Color::premultiply_alpha() const {
  return Color(
      this->red * this->alpha,
      this->green * this->alpha,
      this->blue * this->alpha,
      0xFF);
}
std::string Color::to_string() const {
  char buffer[15];
  sprintf(buffer, "#%02X%02X%02X%02X", this->red, this->green, this->blue, this->alpha);
  return buffer;
}

void DisplayBuffer::init_internal_(uint32_t buffer_length) {
  this->buffer_ = new uint8_t[buffer_length];
  if (this->buffer_ == nullptr) {
    ESP_LOGE(TAG, "Could not allocate buffer for display!");
    return;
  }
  this->clear();
}
void DisplayBuffer::fill(Color color) { this->filled_rectangle(0, 0, this->get_width(), this->get_height(), color); }
int DisplayBuffer::get_width() {
  switch (this->rotation_) {
    case DISPLAY_ROTATION_90_DEGREES:
    case DISPLAY_ROTATION_270_DEGREES:
      return this->get_height_internal();
    case DISPLAY_ROTATION_0_DEGREES:
    case DISPLAY_ROTATION_180_DEGREES:
    default:
      return this->get_width_internal();
  }
}
int DisplayBuffer::get_height() {
  switch (this->rotation_) {
    case DISPLAY_ROTATION_0_DEGREES:
    case DISPLAY_ROTATION_180_DEGREES:
      return this->get_height_internal();
    case DISPLAY_ROTATION_90_DEGREES:
    case DISPLAY_ROTATION_270_DEGREES:
    default:
      return this->get_width_internal();
  }
}
void DisplayBuffer::set_rotation(DisplayRotation rotation) { this->rotation_ = rotation; }
void HOT DisplayBuffer::draw_pixel_at(int x, int y, Color color) {
  switch (this->rotation_) {
    case DISPLAY_ROTATION_0_DEGREES:
      break;
    case DISPLAY_ROTATION_90_DEGREES:
      std::swap(x, y);
      x = this->get_width_internal() - x - 1;
      break;
    case DISPLAY_ROTATION_180_DEGREES:
      x = this->get_width_internal() - x - 1;
      y = this->get_height_internal() - y - 1;
      break;
    case DISPLAY_ROTATION_270_DEGREES:
      std::swap(x, y);
      y = this->get_height_internal() - y - 1;
      break;
  }
  this->draw_absolute_pixel_internal(x, y, color);
  feed_wdt();
}
void DisplayBuffer::draw_pixel_at(int x, int y) {
  this->draw_pixel_at(x, y, this->get_default_color());
}
void HOT DisplayBuffer::line(int x1, int y1, int x2, int y2, Color color) {
  const int32_t dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
  const int32_t dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
  int32_t err = dx + dy;

  while (true) {
    this->draw_pixel_at(x1, y1, color);
    if (x1 == x2 && y1 == y2)
      break;
    int32_t e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x1 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y1 += sy;
    }
  }
}
void DisplayBuffer::line(int x1, int y1, int x2, int y2) {
  this->line(x1, y1, x2, y2, this->get_default_color());
}
void HOT DisplayBuffer::horizontal_line(int x, int y, int width, Color color) {
  // Future: Could be made more efficient by manipulating buffer directly in certain rotations.
  for (int i = x; i < x + width; i++)
    this->draw_pixel_at(i, y, color);
}
void DisplayBuffer::horizontal_line(int x, int y, int width) {
  this->horizontal_line(x, y, width, this->get_default_color());
}
void HOT DisplayBuffer::vertical_line(int x, int y, int height, Color color) {
  // Future: Could be made more efficient by manipulating buffer directly in certain rotations.
  for (int i = y; i < y + height; i++)
    this->draw_pixel_at(x, i, color);
}
void DisplayBuffer::vertical_line(int x, int y, int height) {
  this->horizontal_line(x, y, height, this->get_default_color());
}
void DisplayBuffer::rectangle(int x1, int y1, int width, int height, Color color) {
  this->horizontal_line(x1, y1, width, color);
  this->horizontal_line(x1, y1 + height - 1, width, color);
  this->vertical_line(x1, y1, height, color);
  this->vertical_line(x1 + width - 1, y1, height, color);
}
void DisplayBuffer::rectangle(int x1, int y1, int width, int height) {
  this->rectangle(x1, y1, width, height, this->get_default_color());
}
void DisplayBuffer::filled_rectangle(int x1, int y1, int width, int height, Color color) {
  // Future: Use vertical_line and horizontal_line methods depending on rotation to reduce memory accesses.
  for (int i = y1; i < y1 + height; i++) {
    this->horizontal_line(x1, i, width, color);
  }
}
void DisplayBuffer::filled_rectangle(int x1, int y1, int width, int height) {
  this->filled_rectangle(x1, y1, width, height, this->get_default_color());
}
void HOT DisplayBuffer::circle(int center_x, int center_y, int radius, Color color) {
  int dx = -radius;
  int dy = 0;
  int err = 2 - 2 * radius;
  int e2;

  do {
    this->draw_pixel_at(center_x - dx, center_y + dy, color);
    this->draw_pixel_at(center_x + dx, center_y + dy, color);
    this->draw_pixel_at(center_x + dx, center_y - dy, color);
    this->draw_pixel_at(center_x - dx, center_y - dy, color);
    e2 = err;
    if (e2 < dy) {
      err += ++dy * 2 + 1;
      if (-dx == dy && e2 <= dx) {
        e2 = 0;
      }
    }
    if (e2 > dx) {
      err += ++dx * 2 + 1;
    }
  } while (dx <= 0);
}
void DisplayBuffer::circle(int center_x, int center_y, int radius) {
  this->circle(center_x, center_y, radius, this->get_default_color());
}
void DisplayBuffer::filled_circle(int center_x, int center_y, int radius, Color color) {
  int dx = -int32_t(radius);
  int dy = 0;
  int err = 2 - 2 * radius;
  int e2;

  do {
    this->draw_pixel_at(center_x - dx, center_y + dy, color);
    this->draw_pixel_at(center_x + dx, center_y + dy, color);
    this->draw_pixel_at(center_x + dx, center_y - dy, color);
    this->draw_pixel_at(center_x - dx, center_y - dy, color);
    int hline_width = 2 * (-dx) + 1;
    this->horizontal_line(center_x + dx, center_y + dy, hline_width, color);
    this->horizontal_line(center_x + dx, center_y - dy, hline_width, color);
    e2 = err;
    if (e2 < dy) {
      err += ++dy * 2 + 1;
      if (-dx == dy && e2 <= dx) {
        e2 = 0;
      }
    }
    if (e2 > dx) {
      err += ++dx * 2 + 1;
    }
  } while (dx <= 0);
}
void DisplayBuffer::filled_circle(int center_x, int center_y, int radius) {
  this->filled_circle(center_x, center_y, radius, this->get_default_color());
}
void DisplayBuffer::print(int x, int y, Font *font, Color color, TextAlign align, const char *text) {
  int x_start, y_start;
  int width, height;
  this->get_text_bounds(x, y, text, font, align, &x_start, &y_start, &width, &height);

  int i = 0;
  int x_at = x_start;
  while (text[i] != '\0') {
    int match_length;
    int glyph_n = font->match_next_glyph(text + i, &match_length);
    if (glyph_n < 0) {
      // Unknown char, skip
      ESP_LOGW(TAG, "Encountered character without representation in font: '%c'", text[i]);
      if (!font->get_glyphs().empty()) {
        uint8_t glyph_width = font->get_glyphs()[0].width_;
        for (int glyph_x = 0; glyph_x < glyph_width; glyph_x++)
          for (int glyph_y = 0; glyph_y < height; glyph_y++)
            this->draw_pixel_at(glyph_x + x_at, glyph_y + y_start, color);
        x_at += glyph_width;
      }

      i++;
      continue;
    }

    const Glyph &glyph = font->get_glyphs()[glyph_n];
    int scan_x1, scan_y1, scan_width, scan_height;
    glyph.scan_area(&scan_x1, &scan_y1, &scan_width, &scan_height);

    for (int glyph_x = scan_x1; glyph_x < scan_x1 + scan_width; glyph_x++) {
      for (int glyph_y = scan_y1; glyph_y < scan_y1 + scan_height; glyph_y++) {
        if (glyph.get_pixel(glyph_x, glyph_y)) {
          this->draw_pixel_at(glyph_x + x_at, glyph_y + y_start, color);
        }
      }
    }

    x_at += glyph.width_ + glyph.offset_x_;

    i += match_length;
  }
}
void DisplayBuffer::vprintf_(int x, int y, Font *font, Color color, TextAlign align, const char *format, va_list arg) {
  char buffer[256];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  if (ret > 0)
    this->print(x, y, font, color, align, buffer);
}
void DisplayBuffer::image(int x, int y, Image *image) {
  for (int img_x = 0; img_x < image->get_width(); img_x++) {
    for (int img_y = 0; img_y < image->get_height(); img_y++) {
      this->draw_pixel_at(x + img_x, y + img_y, image->get_pixel(img_x, img_y) ? COLOR_ON : COLOR_OFF);
    }
  }
}
void DisplayBuffer::get_text_bounds(int x, int y, const char *text, Font *font, TextAlign align, int *x1, int *y1,
                                    int *width, int *height) {
  int x_offset, baseline;
  font->measure(text, width, &x_offset, &baseline, height);

  auto x_align = TextAlign(int(align) & 0x18);
  auto y_align = TextAlign(int(align) & 0x07);

  switch (x_align) {
    case TextAlign::RIGHT:
      *x1 = x - *width;
      break;
    case TextAlign::CENTER_HORIZONTAL:
      *x1 = x - (*width) / 2;
      break;
    case TextAlign::LEFT:
    default:
      // LEFT
      *x1 = x;
      break;
  }

  switch (y_align) {
    case TextAlign::BOTTOM:
      *y1 = y - *height;
      break;
    case TextAlign::BASELINE:
      *y1 = y - baseline;
      break;
    case TextAlign::CENTER_VERTICAL:
      *y1 = y - (*height) / 2;
      break;
    case TextAlign::TOP:
    default:
      *y1 = y;
      break;
  }
}
void DisplayBuffer::print(int x, int y, Font *font, Color color, const char *text) {
  this->print(x, y, font, color, TextAlign::TOP_LEFT, text);
}
void DisplayBuffer::print(int x, int y, Font *font, TextAlign align, const char *text) {
  this->print(x, y, font, this->get_default_color(), align, text);
}
void DisplayBuffer::print(int x, int y, Font *font, const char *text) {
  this->print(x, y, font, this->get_default_color(), TextAlign::TOP_LEFT, text);
}
void DisplayBuffer::printf(int x, int y, Font *font, Color color, TextAlign align, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  this->vprintf_(x, y, font, color, align, format, arg);
  va_end(arg);
}
void DisplayBuffer::printf(int x, int y, Font *font, Color color, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  this->vprintf_(x, y, font, color, TextAlign::TOP_LEFT, format, arg);
  va_end(arg);
}
void DisplayBuffer::printf(int x, int y, Font *font, TextAlign align, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  this->vprintf_(x, y, font, this->get_default_color(), align, format, arg);
  va_end(arg);
}
void DisplayBuffer::printf(int x, int y, Font *font, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  this->vprintf_(x, y, font, this->get_default_color(), TextAlign::CENTER_LEFT, format, arg);
  va_end(arg);
}
void DisplayBuffer::set_writer(display_writer_t &&writer) { this->writer_ = writer; }
void DisplayBuffer::set_pages(std::vector<DisplayPage *> pages) {
  for (auto *page : pages)
    page->set_parent(this);

  for (uint32_t i = 0; i < pages.size() - 1; i++) {
    pages[i]->set_next(pages[i + 1]);
    pages[i + 1]->set_prev(pages[i]);
  }
  pages[0]->set_prev(pages[pages.size() - 1]);
  pages[pages.size() - 1]->set_next(pages[0]);
  this->show_page(pages[0]);
}
void DisplayBuffer::show_page(DisplayPage *page) { this->page_ = page; }
void DisplayBuffer::show_next_page() { this->page_->show_next(); }
void DisplayBuffer::show_prev_page() { this->page_->show_prev(); }
void DisplayBuffer::do_update_() {
  this->clear();
  if (this->page_ != nullptr) {
    this->page_->get_writer()(*this);
  } else if (this->writer_.has_value()) {
    (*this->writer_)(*this);
  }
}
#ifdef USE_TIME
void DisplayBuffer::strftime(int x, int y, Font *font, Color color, TextAlign align, const char *format,
                             time::ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    this->print(x, y, font, color, align, buffer);
}
void DisplayBuffer::strftime(int x, int y, Font *font, Color color, const char *format, time::ESPTime time) {
  this->strftime(x, y, font, color, TextAlign::TOP_LEFT, format, time);
}
void DisplayBuffer::strftime(int x, int y, Font *font, TextAlign align, const char *format, time::ESPTime time) {
  this->strftime(x, y, font, this->get_default_color(), align, format, time);
}
void DisplayBuffer::strftime(int x, int y, Font *font, const char *format, time::ESPTime time) {
  this->strftime(x, y, font, this->get_default_color(), TextAlign::TOP_LEFT, format, time);
}
#endif

Glyph::Glyph(const char *a_char, const uint8_t *data_start, uint32_t offset, int offset_x, int offset_y, int width,
             int height)
    : char_(a_char),
      data_(data_start + offset),
      offset_x_(offset_x),
      offset_y_(offset_y),
      width_(width),
      height_(height) {}
bool Glyph::get_pixel(int x, int y) const {
  const int x_data = x - this->offset_x_;
  const int y_data = y - this->offset_y_;
  if (x_data < 0 || x_data >= this->width_ || y_data < 0 || y_data >= this->height_)
    return false;
  const uint32_t width_8 = ((this->width_ + 7u) / 8u) * 8u;
  const uint32_t pos = x_data + y_data * width_8;
  return pgm_read_byte(this->data_ + (pos / 8u)) & (0x80 >> (pos % 8u));
}
const char *Glyph::get_char() const { return this->char_; }
bool Glyph::compare_to(const char *str) const {
  // 1 -> this->char_
  // 2 -> str
  for (uint32_t i = 0;; i++) {
    if (this->char_[i] == '\0')
      return true;
    if (str[i] == '\0')
      return false;
    if (this->char_[i] > str[i])
      return false;
    if (this->char_[i] < str[i])
      return true;
  }
  // this should not happen
  return false;
}
int Glyph::match_length(const char *str) const {
  for (uint32_t i = 0;; i++) {
    if (this->char_[i] == '\0')
      return i;
    if (str[i] != this->char_[i])
      return 0;
  }
  // this should not happen
  return 0;
}
void Glyph::scan_area(int *x1, int *y1, int *width, int *height) const {
  *x1 = this->offset_x_;
  *y1 = this->offset_y_;
  *width = this->width_;
  *height = this->height_;
}
int Font::match_next_glyph(const char *str, int *match_length) {
  int lo = 0;
  int hi = this->glyphs_.size() - 1;
  while (lo != hi) {
    int mid = (lo + hi + 1) / 2;
    if (this->glyphs_[mid].compare_to(str))
      lo = mid;
    else
      hi = mid - 1;
  }
  *match_length = this->glyphs_[lo].match_length(str);
  if (*match_length <= 0)
    return -1;
  return lo;
}
void Font::measure(const char *str, int *width, int *x_offset, int *baseline, int *height) {
  *baseline = this->baseline_;
  *height = this->bottom_;
  int i = 0;
  int min_x = 0;
  bool has_char = false;
  int x = 0;
  while (str[i] != '\0') {
    int match_length;
    int glyph_n = this->match_next_glyph(str + i, &match_length);
    if (glyph_n < 0) {
      // Unknown char, skip
      if (!this->get_glyphs().empty())
        x += this->get_glyphs()[0].width_;
      i++;
      continue;
    }

    const Glyph &glyph = this->glyphs_[glyph_n];
    if (!has_char)
      min_x = glyph.offset_x_;
    else
      min_x = std::min(min_x, x + glyph.offset_x_);
    x += glyph.width_ + glyph.offset_x_;

    i += match_length;
    has_char = true;
  }
  *x_offset = min_x;
  *width = x - min_x;
}
const std::vector<Glyph> &Font::get_glyphs() const { return this->glyphs_; }
Font::Font(std::vector<Glyph> &&glyphs, int baseline, int bottom)
    : glyphs_(std::move(glyphs)), baseline_(baseline), bottom_(bottom) {}

bool Image::get_pixel(int x, int y) const {
  if (x < 0 || x >= this->width_ || y < 0 || y >= this->height_)
    return false;
  const uint32_t width_8 = ((this->width_ + 7u) / 8u) * 8u;
  const uint32_t pos = x + y * width_8;
  return pgm_read_byte(this->data_start_ + (pos / 8u)) & (0x80 >> (pos % 8u));
}
int Image::get_width() const { return this->width_; }
int Image::get_height() const { return this->height_; }
Image::Image(const uint8_t *data_start, int width, int height)
    : width_(width), height_(height), data_start_(data_start) {}

DisplayPage::DisplayPage(const display_writer_t &writer) : writer_(writer) {}
void DisplayPage::show() { this->parent_->show_page(this); }
void DisplayPage::show_next() { this->next_->show(); }
void DisplayPage::show_prev() { this->prev_->show(); }
void DisplayPage::set_parent(DisplayBuffer *parent) { this->parent_ = parent; }
void DisplayPage::set_prev(DisplayPage *prev) { this->prev_ = prev; }
void DisplayPage::set_next(DisplayPage *next) { this->next_ = next; }
const display_writer_t &DisplayPage::get_writer() const { return this->writer_; }

}  // namespace display

ESPHOME_NAMESPACE_END

#endif  // USE_DISPLAY
