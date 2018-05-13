//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/light/light_output_component.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

#ifdef USE_LIGHT

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

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
