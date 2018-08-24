//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/defines.h"

#ifdef USE_LIGHT

#include "esphomelib/light/light_output_component.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

using esphomelib::output::FloatOutput;

LightTraits BinaryLightOutput::get_traits() {
  return {false, false, false};
}
void BinaryLightOutput::write_state(LightState *state) {
  bool value;
  state->current_values_as_binary(&value);
  if (value) this->output_->enable();
  else this->output_->disable();
}
BinaryLightOutput::BinaryLightOutput(output::BinaryOutput *output)
    : LightOutput(), output_(output) {

}

LightTraits MonochromaticLightOutput::get_traits() {
  return {true, false, false};
}
void MonochromaticLightOutput::write_state(LightState *state) {
  float value;
  state->current_values_as_brightness(&value);
  this->output_->set_state_(value);
}
MonochromaticLightOutput::MonochromaticLightOutput(FloatOutput *output)
    : output_(output) {

}

LightTraits CWWWLightOutput::get_traits() {
  return {true, false, false, false, true};
}
void CWWWLightOutput::write_state(LightState *state) {
  float cold_white, warm_white;
  state->current_values_as_cwww(this->cold_white_mireds_, this->warm_white_mireds_,
                                &cold_white, &warm_white);
  this->cold_white_->set_state_(cold_white);
  this->warm_white_->set_state_(warm_white);
}
CWWWLightOutput::CWWWLightOutput(float cold_white_mireds,
                                 float warm_white_mireds,
                                 FloatOutput *cold_white,
                                 FloatOutput *warm_white)
    : cold_white_mireds_(cold_white_mireds), warm_white_mireds_(warm_white_mireds),
      cold_white_(cold_white), warm_white_(warm_white) {

}

LightTraits RGBLightOutput::get_traits() {
  return {true, true, false};
}
void RGBLightOutput::write_state(LightState *state) {
  float red, green, blue;
  state->current_values_as_rgb(&red, &green, &blue);
  this->red_->set_state_(red);
  this->green_->set_state_(green);
  this->blue_->set_state_(blue);
}
RGBLightOutput::RGBLightOutput(FloatOutput *red, FloatOutput *green, FloatOutput *blue)
    : red_(red), green_(green), blue_(blue) {

}

LightTraits RGBWLightOutput::get_traits() {
  return {true, true, true};
}
void RGBWLightOutput::write_state(LightState *state) {
  float red, green, blue, white;
  state->current_values_as_rgbw(&red, &green, &blue, &white);
  this->red_->set_state_(red);
  this->green_->set_state_(green);
  this->blue_->set_state_(blue);
  this->white_->set_state_(white);
}
RGBWLightOutput::RGBWLightOutput(FloatOutput *red, FloatOutput *green, FloatOutput *blue, FloatOutput *white)
    : red_(red), green_(green), blue_(blue), white_(white) {

}

LightTraits RGBWWLightOutput::get_traits() {
  return {true, true, true, false, true};
}
void RGBWWLightOutput::write_state(LightState *state) {
  float red, green, blue, cold_white, warm_white;
  state->current_values_as_rgbww(this->cold_white_mireds_, this->warm_white_mireds_,
                                 &red, &green, &blue, &cold_white, &warm_white);
  this->red_->set_state_(red);
  this->green_->set_state_(green);
  this->blue_->set_state_(blue);
  this->cold_white_->set_state_(cold_white);
  this->warm_white_->set_state_(warm_white);
}
RGBWWLightOutput::RGBWWLightOutput(float cold_white_mireds,
                                   float warm_white_mireds,
                                   FloatOutput *red,
                                   FloatOutput *green,
                                   FloatOutput *blue,
                                   FloatOutput *cold_white,
                                   FloatOutput *warm_white)
    : cold_white_mireds_(cold_white_mireds), warm_white_mireds_(warm_white_mireds),
      red_(red), green_(green), blue_(blue),
      cold_white_(cold_white), warm_white_(warm_white) {

}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
