//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/defines.h"

#ifdef USE_LIGHT

#include "esphomelib/light/light_traits.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

LightTraits::LightTraits()
    : brightness_(false), rgb_(false), rgb_white_value_(false) {}

LightTraits::LightTraits(bool brightness, bool rgb, bool rgb_white_value, bool fast_led)
    : brightness_(brightness), rgb_(rgb), rgb_white_value_(rgb_white_value), fast_led_(fast_led) {}

bool LightTraits::supports_traits(const LightTraits &rhs) const {
  return (!rhs.brightness_ || this->brightness_) && (!rhs.rgb_ || this->rgb_) &&
      (!rhs.rgb_white_value_ || this->rgb_white_value_) && (!rhs.fast_led_ || this->fast_led_);
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
bool LightTraits::has_fast_led() const {
  return this->fast_led_;
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
