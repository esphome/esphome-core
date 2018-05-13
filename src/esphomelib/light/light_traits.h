//
// Created by Otto Winter on 28.11.17.
//

#ifndef ESPHOMELIB_LIGHT_LIGHT_TRAITS_H
#define ESPHOMELIB_LIGHT_LIGHT_TRAITS_H

#include <cstdint>
#include "esphomelib/defines.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

/// This class is used to represent the capabilities of a light.
class LightTraits {
 public:
  LightTraits();

  LightTraits(bool brightness, bool rgb, bool rgb_white_value);

  bool supports_brightness() const;
  bool supports_rgb() const;
  bool supports_rgbw() const;
  bool supports_traits(const LightTraits &rhs) const;

  bool has_brightness() const;
  void set_brightness(bool brightness);

  bool has_rgb() const;
  void set_rgb(bool rgb);

  bool has_rgb_white_value() const;
  void set_rgb_white_value(bool rgb_white_value);

 protected:
  bool brightness_;
  bool rgb_;
  bool rgb_white_value_;
};

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_LIGHT_TRAITS_H
