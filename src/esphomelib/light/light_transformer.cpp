//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/light/light_transformer.h"

#include "esphomelib/helpers.h"
#include "esphomelib/component.h"
#include "esphomelib/log.h"
#include "esphomelib/esphal.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

const LightColorValues &LightTransformer::get_start_values() const {
  return start_values_;
}

const LightColorValues &LightTransformer::get_target_values() const {
  return target_values_;
}

LightTransformer::LightTransformer(uint32_t start_time, uint32_t length,
                                   const LightColorValues &start_values,
                                   const LightColorValues &target_values) : start_time_(
    start_time), length_(length), start_values_(start_values), target_values_(target_values) {}

bool LightTransformer::is_finished() {
  return this->get_progress() >= 1.0f;
}

float LightTransformer::get_progress() {
  return clamp(0.0f, 1.0f, (millis() - this->start_time_) / float(this->length_));
}

LightColorValues LightTransformer::get_remote_values() {
  return this->get_target_values();
}

LightColorValues LightTransformer::get_end_values() {
  return this->get_target_values();
}

LightColorValues LightTransitionTransformer::get_values() {
  float x = this->get_progress();
  float v = x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
  return LightColorValues::lerp(this->get_start_values(), this->get_target_values(), v);
}
LightTransitionTransformer::LightTransitionTransformer(uint32_t start_time,
                                                       uint32_t length,
                                                       const LightColorValues &start_values,
                                                       const LightColorValues &target_values) :
    LightTransformer(start_time, length, start_values, target_values) {}

LightColorValues LightFlashTransformer::get_values() {
  return this->get_target_values();
}

LightColorValues LightFlashTransformer::get_end_values() {
  return this->get_start_values();
}

LightFlashTransformer::LightFlashTransformer(uint32_t start_time, uint32_t length,
                                             const LightColorValues &start_values,
                                             const LightColorValues &target_values)
    : LightTransformer(start_time, length, start_values, target_values) {}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
