#include "esphomelib/defines.h"

#ifdef USE_LIGHT

#include "esphomelib/light/light_traits.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

LightTraits::LightTraits()
    : brightness_(false), rgb_(false), rgb_white_value_(false) {}

LightTraits::LightTraits(bool brightness, bool rgb, bool rgb_white_value, bool color_temperature)
    : brightness_(brightness), rgb_(rgb), rgb_white_value_(rgb_white_value),
      color_temperature_(color_temperature) {}

bool LightTraits::has_brightness() const {
  return this->brightness_;
}
bool LightTraits::has_rgb() const {
  return this->rgb_;
}
bool LightTraits::has_rgb_white_value() const {
  return this->rgb_white_value_;
}
bool LightTraits::has_color_temperature() const {
  return this->color_temperature_;
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
