#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/light/light_effect.h"
#include "esphome/helpers.h"
#include "esphome/esphal.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

void RandomLightEffect::apply() {
  const uint32_t now = millis();
  if (now - this->last_color_change_ < this->update_interval_) {
    return;
  }
  auto call = this->state_->turn_on();
  call.set_red_if_supported(random_float());
  call.set_green_if_supported(random_float());
  call.set_blue_if_supported(random_float());
  call.set_white_if_supported(random_float());
  call.set_color_temperature_if_supported(random_float());
  call.set_transition_length_if_supported(this->transition_length_);
  call.set_publish(true);
  call.set_save(false);
  call.perform();

  this->last_color_change_ = now;
}

RandomLightEffect::RandomLightEffect(const std::string &name) : LightEffect(name) {}

void RandomLightEffect::set_transition_length(uint32_t transition_length) {
  this->transition_length_ = transition_length;
}
void RandomLightEffect::set_update_interval(uint32_t update_interval) { this->update_interval_ = update_interval; }

LightEffect::LightEffect(const std::string &name) : name_(name) {}

void LightEffect::start() {}
void LightEffect::start_internal() { this->start(); }
void LightEffect::stop() {}
const std::string &LightEffect::get_name() { return this->name_; }
void LightEffect::init() {}
void LightEffect::init_internal(LightState *state) {
  this->state_ = state;
  this->init();
}

LambdaLightEffect::LambdaLightEffect(const std::string &name, const std::function<void()> &f, uint32_t update_interval)
    : LightEffect(name), f_(f), update_interval_(update_interval) {}

void LambdaLightEffect::apply() {
  const uint32_t now = millis();
  if (now - this->last_run_ >= this->update_interval_) {
    this->last_run_ = now;
    this->f_();
  }
}

void StrobeLightEffect::apply() {
  const uint32_t now = millis();
  if (now - this->last_switch_ < this->colors_[this->at_color_].duration)
    return;

  // Switch to next color
  this->at_color_ = (this->at_color_ + 1) % this->colors_.size();
  auto color = this->colors_[this->at_color_].color;

  auto call = this->state_->turn_on();
  call.from_light_color_values(this->colors_[this->at_color_].color);

  if (!color.is_on()) {
    // Don't turn the light off, otherwise the light effect will be stopped
    call.set_brightness_if_supported(0.0f);
    call.set_state(true);
  }
  call.set_publish(false);
  call.set_save(false);
  call.set_transition_length_if_supported(0);
  call.perform();
  this->last_switch_ = now;
}
StrobeLightEffect::StrobeLightEffect(const std::string &name) : LightEffect(name) {
  this->colors_.reserve(2);
  this->colors_.push_back(
      StrobeLightEffectColor{.color = LightColorValues(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f), .duration = 500});
  this->colors_.push_back(
      StrobeLightEffectColor{.color = LightColorValues(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f), .duration = 500});
}

void StrobeLightEffect::set_colors(const std::vector<StrobeLightEffectColor> &colors) { this->colors_ = colors; }

inline float random_cubic_float() {
  const float r = random_float() * 2.0f - 1.0f;
  return r * r * r;
}

void FlickerLightEffect::apply() {
  LightColorValues remote = this->state_->remote_values;
  LightColorValues current = this->state_->current_values;
  LightColorValues out;
  const float alpha = this->alpha_;
  const float beta = 1.0f - alpha;
  out.set_state(remote.get_state());
  out.set_brightness(remote.get_brightness() * beta + current.get_brightness() * alpha +
                     (random_cubic_float() * this->intensity_));
  out.set_red(remote.get_red() * beta + current.get_red() * alpha + (random_cubic_float() * this->intensity_));
  out.set_green(remote.get_green() * beta + current.get_green() * alpha + (random_cubic_float() * this->intensity_));
  out.set_blue(remote.get_blue() * beta + current.get_blue() * alpha + (random_cubic_float() * this->intensity_));
  out.set_white(remote.get_white() * beta + current.get_white() * alpha + (random_cubic_float() * this->intensity_));

  auto traits = this->state_->get_traits();
  auto call = this->state_->make_call();
  call.set_publish(false);
  call.set_save(false);
  if (traits.has_brightness())
    call.set_transition_length(0);
  call.from_light_color_values(out);
  call.perform();
}
void FlickerLightEffect::set_alpha(float alpha) { this->alpha_ = alpha; }
void FlickerLightEffect::set_intensity(float intensity) { this->intensity_ = intensity; }
FlickerLightEffect::FlickerLightEffect(const std::string &name) : LightEffect(name) {}

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT
