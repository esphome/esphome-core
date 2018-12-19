#ifndef ESPHOMELIB_LIGHT_LIGHT_TRAITS_H
#define ESPHOMELIB_LIGHT_LIGHT_TRAITS_H

#include "esphomelib/defines.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

/// This class is used to represent the capabilities of a light.
class LightTraits {
 public:
  LightTraits();

  LightTraits(bool brightness, bool rgb, bool rgb_white_value, bool fast_led = false, bool color_temperature = false);

  bool has_brightness() const;
  bool has_rgb() const;
  bool has_rgb_white_value() const;
  bool has_color_temperature() const;
  /// Hack to allow FastLED light effects without dynamic_cast.
  bool has_fast_led() const;
  float get_min_mireds() const;
  float get_max_mireds() const;
  void set_min_mireds(float min_mireds);
  void set_max_mireds(float max_mireds);

 protected:
  bool brightness_{false};
  bool rgb_{false};
  bool rgb_white_value_{false};
  bool color_temperature_{false};
  bool fast_led_{false};
  float min_mireds_{0};
  float max_mireds_{0};
};

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_LIGHT_TRAITS_H
