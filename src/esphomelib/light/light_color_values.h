//
// Created by Otto Winter on 28.11.17.
//

#ifndef ESPHOMELIB_LIGHT_LIGHT_COLOR_VALUES_H
#define ESPHOMELIB_LIGHT_LIGHT_COLOR_VALUES_H

#include <ArduinoJson.h>
#include <string>

#include "esphomelib/light/light_traits.h"

namespace esphomelib {

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

  LightColorValues(float state, float brightness, float red, float green, float blue, float white);

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

  /** Load the color values from the non-volatile storage container preferences into the this object.
   *
   * @param friendly_name The friendly name of the component that's calling this.
   * @see save_to_preferences()
   */
  void load_from_preferences(const std::string &friendly_name);

  /** Store these LightColorValues in the preferences object.
   *
   * @param friendly_name The friendly name of the component that's calling this.
   * @see load_from_preferences()
   */
  void save_to_preferences(const std::string &friendly_name) const;

  /** Parse a color from the provided JsonObject.
   *
   * See <a href="https://home-assistant.io/components/light.mqtt_json/">Home Assistant MQTT JSON light</a>.
   *
   * @param root The json root object.
   */
  void parse_json(const JsonObject &root);

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

  /// Compare this LightColorValues to rhs, return true iff all attributes match.
  bool operator==(const LightColorValues &rhs) const;
  bool operator!=(const LightColorValues &rhs) const;

  float get_state() const;
  void set_state(float state);

  float get_brightness() const;
  void set_brightness(float brightness);

  float get_red() const;
  void set_red(float red);

  float get_green() const;
  void set_green(float green);

  float get_blue() const;
  void set_blue(float blue);

  float get_white() const;
  void set_white(float white);

 protected:
  float state_; ///< ON / OFF, float for transition
  float brightness_;
  float red_;
  float green_;
  float blue_;
  float white_;
};

} // namespace light

} // namespace esphomelib


#endif //ESPHOMELIB_LIGHT_LIGHT_COLOR_VALUES_H
