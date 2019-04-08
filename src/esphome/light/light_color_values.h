#ifndef ESPHOME_LIGHT_LIGHT_COLOR_VALUES_H
#define ESPHOME_LIGHT_LIGHT_COLOR_VALUES_H

#include "esphome/defines.h"

#ifdef USE_LIGHT

#include <ArduinoJson.h>
#include <string>
#include "esphome/light/light_traits.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

/** This class represents the color state for a light object.
 *
 * All values in this class are represented using floats in the range from 0.0 (off) to 1.0 (on).
 * Not all values have to be populated though, for example a simple monochromatic light only needs
 * to access the state and brightness attributes.
 *
 * PLease note all float values are automatically clamped.
 *
 * state - Whether the light should be on/off. Represented as a float for transitions.
 * brightness - The brightness of the light.
 * red, green, blue - RGB values.
 * white - The white value for RGBW lights.
 */
class LightColorValues {
 public:
  /// Construct the LightColorValues with all attributes enabled, but state set to 0.0
  LightColorValues();

  LightColorValues(float state, float brightness, float red, float green, float blue, float white,
                   float color_temperature = 1.0f);

  LightColorValues(bool state, float brightness, float red, float green, float blue, float white,
                   float color_temperature = 1.0f);

  /// Create light color values from a binary true/false state.
  static LightColorValues from_binary(bool state);

  /// Create light color values from a monochromatic brightness state.
  static LightColorValues from_monochromatic(float brightness);

  /// Create light color values from an RGB state.
  static LightColorValues from_rgb(float r, float g, float b);

  /// Create light color values from an RGBW state.
  static LightColorValues from_rgbw(float r, float g, float b, float w);

  /** Linearly interpolate between the values in start to the values in end.
   *
   * This function linearly interpolates the color value by just interpolating every attribute
   * independently.
   *
   * @param start The interpolation start values.
   * @param end The interpolation end values.
   * @param completion The completion value. 0 -> start, 1 -> end.
   * @return The linearly interpolated LightColorValues.
   */
  static LightColorValues lerp(const LightColorValues &start, const LightColorValues &end, float completion);

  /** Dump this color into a JsonObject. Only dumps values if the corresponding traits are marked supported by traits.
   *
   * @param root The json root object.
   * @param traits The traits object used for determining whether to include certain attributes.
   */
  void dump_json(JsonObject &root, const LightTraits &traits) const;

  /** Normalize the color (RGB/W) component.
   *
   * Divides all color attributes by the maximum attribute, so effectively set at least one attribute to 1.
   * For example: r=0.3, g=0.5, b=0.4 => r=0.6, g=1.0, b=0.8
   *
   * @param traits Used for determining which attributes to consider.
   */
  void normalize_color(const LightTraits &traits);

  /// Convert these light color values to a binary representation and write them to binary.
  void as_binary(bool *binary) const;

  /// Convert these light color values to a brightness-only representation and write them to brightness.
  void as_brightness(float *brightness) const;

  /// Convert these light color values to an RGB representation and write them to red, green, blue.
  void as_rgb(float *red, float *green, float *blue) const;

  /// Convert these light color values to an RGBW representation and write them to red, green, blue, white.
  void as_rgbw(float *red, float *green, float *blue, float *white) const;

  /// Convert these light color values to an RGBWW representation with the given parameters.
  void as_rgbww(float color_temperature_cw, float color_temperature_ww, float *red, float *green, float *blue,
                float *cold_white, float *warm_white) const;

  /// Convert these light color values to an CWWW representation with the given parameters.
  void as_cwww(float color_temperature_cw, float color_temperature_ww, float *cold_white, float *warm_white) const;

  /// Compare this LightColorValues to rhs, return true if and only if all attributes match.
  bool operator==(const LightColorValues &rhs) const;
  bool operator!=(const LightColorValues &rhs) const;

  /// Get the state of these light color values. In range from 0.0 (off) to 1.0 (on)
  float get_state() const;
  /// Get the binary true/false state of these light color values.
  bool is_on() const;
  /// Set the state of these light color values. In range from 0.0 (off) to 1.0 (on)
  void set_state(float state);
  /// Set the state of these light color values as a binary true/false.
  void set_state(bool state);

  /// Get the brightness property of these light color values. In range 0.0 to 1.0
  float get_brightness() const;
  /// Set the brightness property of these light color values. In range 0.0 to 1.0
  void set_brightness(float brightness);

  /// Get the red property of these light color values. In range 0.0 to 1.0
  float get_red() const;
  /// Set the red property of these light color values. In range 0.0 to 1.0
  void set_red(float red);

  /// Get the green property of these light color values. In range 0.0 to 1.0
  float get_green() const;
  /// Set the green property of these light color values. In range 0.0 to 1.0
  void set_green(float green);

  /// Get the blue property of these light color values. In range 0.0 to 1.0
  float get_blue() const;
  /// Set the blue property of these light color values. In range 0.0 to 1.0
  void set_blue(float blue);

  /// Get the white property of these light color values. In range 0.0 to 1.0
  float get_white() const;
  /// Set the white property of these light color values. In range 0.0 to 1.0
  void set_white(float white);

  /// Get the color temperature property of these light color values in mired.
  float get_color_temperature() const;
  /// Set the color temperature property of these light color values in mired.
  void set_color_temperature(float color_temperature);

 protected:
  float state_;  ///< ON / OFF, float for transition
  float brightness_;
  float red_;
  float green_;
  float blue_;
  float white_;
  float color_temperature_;  ///< Color Temperature in Mired
};

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT

#endif  // ESPHOME_LIGHT_LIGHT_COLOR_VALUES_H
