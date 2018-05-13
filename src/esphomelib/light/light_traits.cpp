//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/light/light_traits.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

bool LightTraits::supports_brightness() const {
  return this->brightness_;
}

bool LightTraits::supports_rgb() const {
  return this->supports_brightness() && this->rgb_;
}

bool LightTraits::supports_rgbw() const {
  return this->supports_rgb() && this->rgb_white_value_;
}

LightTraits::LightTraits()
    : brightness_(false), rgb_(false), rgb_white_value_(false) {}

LightTraits::LightTraits(bool brightness, bool rgb, bool rgb_white_value)
    : brightness_(brightness), rgb_(rgb), rgb_white_value_(rgb_white_value) {}

bool LightTraits::supports_traits(const LightTraits &rhs) const {
  return (!rhs.brightness_ || this->brightness_) && (!rhs.rgb_ || this->rgb_) &&
      (!rhs.rgb_white_value_ || this->rgb_white_value_);
}
bool LightTraits::has_brightness() const {
  return this->brightness_;
}
bool LightTraits::has_rgb() const {
  return this->rgb_;
}
bool LightTraits::has_rgb_white_value() const {
  return this->rgb_white_value_;
}
void LightTraits::set_brightness(bool brightness) {
  this->brightness_ = brightness;
}
void LightTraits::set_rgb(bool rgb) {
  this->rgb_ = rgb;
}
void LightTraits::set_rgb_white_value(bool rgb_white_value) {
  this->rgb_white_value_ = rgb_white_value;
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
