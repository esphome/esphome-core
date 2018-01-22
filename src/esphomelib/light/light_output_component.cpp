//
// Created by Otto Winter on 28.11.17.
//

#include <esphomelib/helpers.h>
#include <esphomelib/log.h>
#include "light_output_component.h"

namespace esphomelib {

namespace light {

using esphomelib::output::FloatOutput;

static const char *TAG = "light::light_output";

void LightOutput::set_state(LightState *state) {
  this->state_ = state;
}

LightOutput::LightOutput()
    : state_(nullptr) {

}
LightState *LightOutput::get_state() const {
  return this->state_;
}
LightColorValues LightOutput::get_current_values() {
  assert_not_nullptr(this->state_);
  return this->state_->get_current_values();
}

LinearLightOutputComponent::LinearLightOutputComponent()
    : LightOutput(), gamma_correct_(2.8), monochromatic_(nullptr), red_(nullptr),
      green_(nullptr), blue_(nullptr), white_(nullptr), traits_(LightTraits()) {

}

void LinearLightOutputComponent::set_gamma_correct(float gamma_correct) {
  this->gamma_correct_ = gamma_correct;
}

void LinearLightOutputComponent::setup_monochromatic(FloatOutput *monochromatic) {
  this->monochromatic_ = monochromatic;
  this->traits_ = LightTraits(true, false, false);
}

const LightTraits &LinearLightOutputComponent::get_traits() const {
  return this->traits_;
}

void LinearLightOutputComponent::setup_rgb(FloatOutput *red, FloatOutput *green,
                                           FloatOutput *blue) {
  this->red_ = red;
  this->green_ = green;
  this->blue_ = blue;
  this->traits_ = LightTraits(true, true, false);
}

void LinearLightOutputComponent::setup_rgbw(FloatOutput *red, FloatOutput *green,
                                            FloatOutput *blue, FloatOutput *white) {
  this->setup_rgb(red, green, blue);
  this->white_ = white;
  this->traits_ = LightTraits(true, true, true);
}
void LinearLightOutputComponent::loop() {
  LightColorValues values = this->get_current_values();

  if (this->traits_.has_rgb()) {
    float r = values.get_state() * values.get_brightness() * values.get_red();
    assert_not_nullptr(this->red_);
    this->red_->set_value_f(gamma_correct(r, this->gamma_correct_));

    float g = values.get_state() * values.get_brightness() * values.get_green();
    assert_not_nullptr(this->green_);
    this->green_->set_value_f(gamma_correct(g, this->gamma_correct_));

    float b = values.get_state() * values.get_brightness() * values.get_blue();
    assert_not_nullptr(this->blue_);
    this->blue_->set_value_f(gamma_correct(b, this->gamma_correct_));

    if (this->traits_.has_rgb_white_value()) {
      float w = values.get_state() * values.get_brightness() * values.get_white();
      assert_not_nullptr(this->white_);
      this->white_->set_value_f(gamma_correct(w, this->gamma_correct_));
    }
  } else if (this->traits_.has_brightness()) {
    float v = values.get_state() * values.get_brightness();
    assert_not_nullptr(this->monochromatic_);
    this->monochromatic_->set_value_f(gamma_correct(v, this->gamma_correct_));
  } else {
    bool s = values.get_state() != 0.0f;
    assert_not_nullptr(this->binary_);
    this->binary_->set_value(s);
  }
}
float LinearLightOutputComponent::get_gamma_correct() const {
  return this->gamma_correct_;
}
void LinearLightOutputComponent::setup_binary(output::BinaryOutput *binary) {
  this->binary_ = binary;
  this->traits_ = LightTraits(false, false, false);
}

} // namespace light

} // namespace esphomelib