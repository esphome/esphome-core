#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/light/light_transformer.h"

#include "esphome/helpers.h"
#include "esphome/component.h"
#include "esphome/log.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

const LightColorValues &LightTransformer::get_start_values_() const { return start_values_; }

const LightColorValues &LightTransformer::get_target_values_() const { return target_values_; }

LightTransformer::LightTransformer(uint32_t start_time, uint32_t length, const LightColorValues &start_values,
                                   const LightColorValues &target_values)
    : start_time_(start_time), length_(length), start_values_(start_values), target_values_(target_values) {}

bool LightTransformer::is_finished() { return this->get_progress_() >= 1.0f; }

float LightTransformer::get_progress_() {
  return clamp(0.0f, 1.0f, (millis() - this->start_time_) / float(this->length_));
}

LightColorValues LightTransformer::get_remote_values() { return this->get_target_values_(); }

LightColorValues LightTransformer::get_end_values() { return this->get_target_values_(); }

LightColorValues LightTransitionTransformer::get_values() {
  float x = this->get_progress_();
  float v = x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
  return LightColorValues::lerp(this->get_start_values_(), this->get_target_values_(), v);
}
LightTransitionTransformer::LightTransitionTransformer(uint32_t start_time, uint32_t length,
                                                       const LightColorValues &start_values,
                                                       const LightColorValues &target_values)
    : LightTransformer(start_time, length, start_values, target_values) {
  // When turning light on from off state, use colors from new.
  if (!this->start_values_.is_on() && this->target_values_.is_on()) {
    this->start_values_.set_brightness(0.0f);
    this->start_values_.set_red(target_values.get_red());
    this->start_values_.set_green(target_values.get_green());
    this->start_values_.set_blue(target_values.get_blue());
    this->start_values_.set_white(target_values.get_white());
    this->start_values_.set_color_temperature(target_values.get_color_temperature());
  }
}
LightColorValues LightFlashTransformer::get_values() { return this->get_target_values_(); }

LightColorValues LightFlashTransformer::get_end_values() { return this->get_start_values_(); }

LightFlashTransformer::LightFlashTransformer(uint32_t start_time, uint32_t length, const LightColorValues &start_values,
                                             const LightColorValues &target_values)
    : LightTransformer(start_time, length, start_values, target_values) {}

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT
