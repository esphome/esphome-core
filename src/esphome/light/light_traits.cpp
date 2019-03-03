#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/light/light_traits.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

LightTraits::LightTraits() : brightness_(false), rgb_(false), rgb_white_value_(false), color_temperature_(false) {}

LightTraits::LightTraits(bool brightness, bool rgb, bool rgb_white_value, bool color_temperature)
    : brightness_(brightness), rgb_(rgb), rgb_white_value_(rgb_white_value), color_temperature_(color_temperature) {}

bool LightTraits::has_brightness() const { return this->brightness_; }
bool LightTraits::has_rgb() const { return this->rgb_; }
bool LightTraits::has_rgb_white_value() const { return this->rgb_white_value_; }
bool LightTraits::has_color_temperature() const { return this->color_temperature_; }
float LightTraits::get_min_mireds() const { return this->min_mireds_; }
float LightTraits::get_max_mireds() const { return this->max_mireds_; }
void LightTraits::set_min_mireds(float min_mireds) { this->min_mireds_ = min_mireds; }
void LightTraits::set_max_mireds(float max_mireds) { this->max_mireds_ = max_mireds; }

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT
