//
// Created by Otto Winter on 28.11.17.
//

#ifndef ESPHOMELIB_LIGHT_LIGHT_OUTPUT_COMPONENT_H
#define ESPHOMELIB_LIGHT_LIGHT_OUTPUT_COMPONENT_H

#include <esphomelib/component.h>
#include <esphomelib/output/float_output.h>
#include "light_state.h"
#include "esphomelib/output/float_output.h"

namespace esphomelib {

namespace light {

/// LightOutput - Interface to write LightColorValues to hardware.
class LightOutput {
 public:
  LightOutput();

  void set_state(LightState *state);
  LightState *get_state() const;

  /// Return the LightTraits of this LightCouput.
  virtual const LightTraits &get_traits() const = 0;
  LightColorValues get_current_values();

 protected:
  LightState *state_;
};

/** LinearLightOutputComponent - Enables simple light output to FloatOutputs
 *
 * Supports several light types: monochromatic, RGB, and RGBW. Aditionally supports gamma correction.
 */
class LinearLightOutputComponent : public LightOutput, public Component {
 public:
  /// Construct a LinearLightOutputComponent with 2.8 as the gamma correction factor.
  LinearLightOutputComponent();

  const LightTraits &get_traits() const override;

  float get_gamma_correct() const;
  void set_gamma_correct(float gamma_correct);

  void setup_binary(output::BinaryOutput *binary);

  /// Set up a monochromatic light.
  void setup_monochromatic(output::FloatOutput *monochromatic);

  /// Set up an RGB light.
  void setup_rgb(output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue);

  /// Set up an RGBW light.
  void setup_rgbw(output::FloatOutput *red,
                  output::FloatOutput *green,
                  output::FloatOutput *blue,
                  output::FloatOutput *white);

  void loop() override;

 protected:
  output::BinaryOutput *binary_;
  output::FloatOutput *monochromatic_;
  output::FloatOutput *red_;
  output::FloatOutput *green_;
  output::FloatOutput *blue_;
  output::FloatOutput *white_;
  LightTraits traits_;
  float gamma_correct_;
};

} // namespace light

} // namespace esphomelib

#endif //ESPHOMELIB_LIGHT_LIGHT_OUTPUT_COMPONENT_H
