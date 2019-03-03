#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/light/addressable_light.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

inline static uint8_t esp_scale8(uint8_t i, uint8_t scale) { return (uint16_t(i) * (1 + uint16_t(scale))) / 256; }

ESPColor::ESPColor() : r(0), g(0), b(0), w(0) {  // NOLINT
}

ESPColor::ESPColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white)  // NOLINT
    : r(red), g(green), b(blue), w(white) {}

ESPColor::ESPColor(uint8_t red, uint8_t green, uint8_t blue)  // NOLINT
    : r(red), g(green), b(blue), w(0) {}

ESPColor::ESPColor(uint32_t colorcode)  // NOLINT
    : r((colorcode >> 16) & 0xFF),
      g((colorcode >> 8) & 0xFF),
      b((colorcode >> 0) & 0xFF),
      w((colorcode >> 24) & 0xFF) {}

ESPColor::ESPColor(const ESPColor &rhs) {  // NOLINT
  this->r = rhs.r;
  this->g = rhs.g;
  this->b = rhs.b;
  this->w = rhs.w;
}

ESPColor &ESPColor::operator=(const ESPColor &rhs) {
  this->r = rhs.r;
  this->g = rhs.g;
  this->b = rhs.b;
  this->w = rhs.w;
  return *this;
}

ESPColor &ESPColor::operator=(uint32_t colorcode) {
  this->w = (colorcode >> 24) & 0xFF;
  this->r = (colorcode >> 16) & 0xFF;
  this->g = (colorcode >> 8) & 0xFF;
  this->b = (colorcode >> 0) & 0xFF;
  return *this;
}

ESPColor ESPColor::operator*(const uint8_t scale) const {
  return ESPColor(esp_scale8(this->red, scale), esp_scale8(this->green, scale), esp_scale8(this->blue, scale),
                  esp_scale8(this->white, scale));
}

ESPColor &ESPColor::operator*=(uint8_t scale) {
  this->red = esp_scale8(this->red, scale);
  this->green = esp_scale8(this->green, scale);
  this->blue = esp_scale8(this->blue, scale);
  this->white = esp_scale8(this->white, scale);
  return *this;
}

ESPColor ESPColor::operator+(const ESPColor &add) const {
  ESPColor ret;
  if (uint8_t(add.r + this->r) < this->r)
    ret.r = 255;
  else
    ret.r = this->r + add.r;
  if (uint8_t(add.g + this->g) < this->g)
    ret.g = 255;
  else
    ret.g = this->g + add.g;
  if (uint8_t(add.b + this->b) < this->b)
    ret.b = 255;
  else
    ret.b = this->b + add.b;
  if (uint8_t(add.w + this->w) < this->w)
    ret.w = 255;
  else
    ret.w = this->w + add.w;
  return ret;
}

ESPColor &ESPColor::operator+=(const ESPColor &add) { return *this = (*this) + add; }

ESPColor ESPColor::operator+(uint8_t add) const { return (*this) + ESPColor(add, add, add, add); }

ESPColor &ESPColor::operator+=(uint8_t add) { return *this = (*this) + add; }

ESPColor ESPColor::operator-(const ESPColor &subtract) const {
  ESPColor ret;
  if (subtract.r > this->r)
    ret.r = 0;
  else
    ret.r = this->r - subtract.r;
  if (subtract.g > this->g)
    ret.g = 0;
  else
    ret.g = this->g - subtract.g;
  if (subtract.b > this->b)
    ret.b = 0;
  else
    ret.b = this->b - subtract.b;
  if (subtract.w > this->w)
    ret.w = 0;
  else
    ret.w = this->w - subtract.w;
  return ret;
}

ESPColor &ESPColor::operator-=(const ESPColor &subtract) { return *this = (*this) - subtract; }

ESPColor ESPColor::operator-(uint8_t subtract) const {
  return (*this) - ESPColor(subtract, subtract, subtract, subtract);
}

ESPColor &ESPColor::operator-=(uint8_t subtract) { return *this = (*this) - subtract; }

ESPColor &ESPColor::operator*=(const ESPColor &scale) {
  this->red = esp_scale8(this->red, scale.red);
  this->green = esp_scale8(this->green, scale.green);
  this->blue = esp_scale8(this->blue, scale.blue);
  this->white = esp_scale8(this->white, scale.white);
  return *this;
}

ESPColor ESPColor::operator*(const ESPColor &scale) const {
  return ESPColor(esp_scale8(this->red, scale.red), esp_scale8(this->green, scale.green),
                  esp_scale8(this->blue, scale.blue), esp_scale8(this->white, scale.white));
}

uint8_t &ESPColor::operator[](uint8_t x) { return this->raw[x]; }

bool ESPColor::is_on() { return this->r != 0 || this->g != 0 || this->b != 0 || this->w != 0; }

ESPColorView::ESPColorView(uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white, uint8_t *effect_data,
                           const ESPColorCorrection *color_correction)
    : red_(red),
      green_(green),
      blue_(blue),
      white_(white),
      effect_data_(effect_data),
      color_correction_(color_correction) {}

const ESPColorView &ESPColorView::operator=(const ESPColor &rhs) const {
  this->set(rhs);
  return *this;
}

void ESPColorView::set(const ESPColor &color) const { this->set_rgbw(color.r, color.g, color.b, color.w); }

void ESPColorView::set_red(uint8_t red) const { *this->red_ = this->color_correction_->color_correct_red(red); }

void ESPColorView::set_green(uint8_t green) const {
  *this->green_ = this->color_correction_->color_correct_green(green);
}

void ESPColorView::set_blue(uint8_t blue) const { *this->blue_ = this->color_correction_->color_correct_blue(blue); }

void ESPColorView::set_white(uint8_t white) const {
  if (this->white_ == nullptr)
    return;
  *this->white_ = this->color_correction_->color_correct_white(white);
}

void ESPColorView::set_rgb(uint8_t red, uint8_t green, uint8_t blue) const {
  this->set_red(red);
  this->set_green(green);
  this->set_blue(blue);
}

void ESPColorView::set_rgbw(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) const {
  this->set_rgb(red, green, blue);
  this->set_white(white);
}

ESPColor ESPColorView::get() const {
  return ESPColor(this->get_red(), this->get_green(), this->get_blue(), this->get_white());
}

uint8_t ESPColorView::get_red() const { return this->color_correction_->color_uncorrect_red(*this->red_); }

uint8_t ESPColorView::get_green() const { return this->color_correction_->color_uncorrect_green(*this->green_); }

uint8_t ESPColorView::get_blue() const { return this->color_correction_->color_uncorrect_blue(*this->blue_); }

uint8_t ESPColorView::get_white() const {
  if (this->white_ == nullptr)
    return 0;
  return this->color_correction_->color_uncorrect_white(*this->white_);
}

void ESPColorView::set_effect_data(uint8_t effect_data) const {
  if (this->effect_data_ == nullptr)
    return;
  *this->effect_data_ = effect_data;
}

uint8_t ESPColorView::get_effect_data() const {
  if (this->effect_data_ == nullptr)
    return 0;
  return *this->effect_data_;
}

void ESPColorView::set(const ESPHSVColor &color) const {
  ESPColor rgb = color.to_rgb();
  this->set_rgb(rgb.r, rgb.g, rgb.b);
}

const ESPColorView &ESPColorView::operator=(const ESPHSVColor &rhs) const {
  this->set(rhs);
  return *this;
}
void ESPColorView::raw_set_color_correction(const ESPColorCorrection *color_correction) {
  this->color_correction_ = color_correction;
}

ESPColor ESPColorCorrection::color_correct(ESPColor color) const {
  // corrected = (uncorrected * max_brightness * local_brightness) ^ gamma
  return ESPColor(this->color_correct_red(color.red), this->color_correct_green(color.green),
                  this->color_correct_blue(color.blue), this->color_correct_white(color.white));
}

uint8_t ESPColorCorrection::color_correct_red(uint8_t red) const {
  uint8_t res = esp_scale8(esp_scale8(red, this->max_brightness_.red), this->local_brightness_);
  return this->gamma_table_[res];
}

uint8_t ESPColorCorrection::color_correct_green(uint8_t green) const {
  uint8_t res = esp_scale8(esp_scale8(green, this->max_brightness_.green), this->local_brightness_);
  return this->gamma_table_[res];
}

uint8_t ESPColorCorrection::color_correct_blue(uint8_t blue) const {
  uint8_t res = esp_scale8(esp_scale8(blue, this->max_brightness_.blue), this->local_brightness_);
  return this->gamma_table_[res];
}

uint8_t ESPColorCorrection::color_correct_white(uint8_t white) const {
  // do not scale white value with brightness
  uint8_t res = esp_scale8(white, this->max_brightness_.white);
  return this->gamma_table_[res];
}

ESPColor ESPColorCorrection::color_uncorrect(ESPColor color) const {
  // uncorrected = corrected^(1/gamma) / (max_brightness * local_brightness)
  return ESPColor(this->color_uncorrect_red(color.red), this->color_uncorrect_green(color.green),
                  this->color_uncorrect_blue(color.blue), this->color_uncorrect_white(color.white));
}

uint8_t ESPColorCorrection::color_uncorrect_red(uint8_t red) const {
  if (this->max_brightness_.red == 0 || this->local_brightness_ == 0)
    return 0;
  uint16_t uncorrected = this->gamma_reverse_table_[red] * 255UL;
  uint8_t res = ((uncorrected / this->max_brightness_.red) * 255UL) / this->local_brightness_;
  return res;
}

uint8_t ESPColorCorrection::color_uncorrect_green(uint8_t green) const {
  if (this->max_brightness_.green == 0 || this->local_brightness_ == 0)
    return 0;
  uint16_t uncorrected = this->gamma_reverse_table_[green] * 255UL;
  uint8_t res = ((uncorrected / this->max_brightness_.green) * 255UL) / this->local_brightness_;
  return res;
}

uint8_t ESPColorCorrection::color_uncorrect_blue(uint8_t blue) const {
  if (this->max_brightness_.blue == 0 || this->local_brightness_ == 0)
    return 0;
  uint16_t uncorrected = this->gamma_reverse_table_[blue] * 255UL;
  uint8_t res = ((uncorrected / this->max_brightness_.blue) * 255UL) / this->local_brightness_;
  return res;
}

uint8_t ESPColorCorrection::color_uncorrect_white(uint8_t white) const {
  if (this->max_brightness_.white == 0)
    return 0;
  uint16_t uncorrected = this->gamma_reverse_table_[white] * 255UL;
  uint8_t res = uncorrected / this->max_brightness_.white;
  return res;
}

ESPHSVColor::ESPHSVColor() : h(0), s(0), v(0) {  // NOLINT
}

ESPHSVColor::ESPHSVColor(uint8_t hue, uint8_t saturation, uint8_t value)  // NOLINT
    : hue(hue), saturation(saturation), value(value) {}

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT
