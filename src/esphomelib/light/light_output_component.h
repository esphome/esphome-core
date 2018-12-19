#ifndef ESPHOMELIB_LIGHT_LIGHT_OUTPUT_COMPONENT_H
#define ESPHOMELIB_LIGHT_LIGHT_OUTPUT_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_LIGHT

#include "esphomelib/component.h"
#include "esphomelib/output/float_output.h"
#include "esphomelib/light/light_state.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

class BinaryLightOutput : public LightOutput {
 public:
  explicit BinaryLightOutput(output::BinaryOutput *output);
  LightTraits get_traits() override;
  void write_state(LightState *state) override;

 protected:
  output::BinaryOutput *output_;
};

class MonochromaticLightOutput : public LightOutput {
 public:
  explicit MonochromaticLightOutput(output::FloatOutput *output);
  LightTraits get_traits() override;
  void write_state(LightState *state) override;

 protected:
  output::FloatOutput *output_;
};

class CWWWLightOutput : public LightOutput {
 public:
  CWWWLightOutput(float cold_white_mireds,
                  float warm_white_mireds,
                  output::FloatOutput *cold_white,
                  output::FloatOutput *warm_white);

  LightTraits get_traits() override;
  void write_state(LightState *state) override;

 protected:
  float cold_white_mireds_;
  float warm_white_mireds_;
  output::FloatOutput *cold_white_;
  output::FloatOutput *warm_white_;
};

class RGBLightOutput : public LightOutput {
 public:
  RGBLightOutput(output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue);
  LightTraits get_traits() override;
  void write_state(LightState *state) override;

 protected:
  output::FloatOutput *red_;
  output::FloatOutput *green_;
  output::FloatOutput *blue_;
};

class RGBWLightOutput : public LightOutput {
 public:
  RGBWLightOutput(output::FloatOutput *red,
                  output::FloatOutput *green,
                  output::FloatOutput *blue,
                  output::FloatOutput *white);
  LightTraits get_traits() override;
  void write_state(LightState *state) override;

 protected:
  output::FloatOutput *red_;
  output::FloatOutput *green_;
  output::FloatOutput *blue_;
  output::FloatOutput *white_;
};

class RGBWWLightOutput : public LightOutput {
 public:
  RGBWWLightOutput(float cold_white_mireds,
                   float warm_white_mireds,
                   output::FloatOutput *red,
                   output::FloatOutput *green,
                   output::FloatOutput *blue,
                   output::FloatOutput *cold_white,
                   output::FloatOutput *warm_white);

  LightTraits get_traits() override;
  void write_state(LightState *state) override;

 protected:
  float cold_white_mireds_;
  float warm_white_mireds_;
  output::FloatOutput *red_;
  output::FloatOutput *green_;
  output::FloatOutput *blue_;
  output::FloatOutput *cold_white_;
  output::FloatOutput *warm_white_;
};

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_LIGHT_OUTPUT_COMPONENT_H
