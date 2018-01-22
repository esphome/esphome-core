//
// Created by Otto Winter on 28.11.17.
//

#include <esphomelib/helpers.h>
#include <esphomelib/log.h>
#include <esphomelib/hal.h>
#include "light_state.h"
#include "light_transformer.h"
#include "light_effect.h"

namespace esphomelib {

namespace light {

static const char *TAG = "light::light_state";

void LightState::start_transition(const LightColorValues &target, uint32_t length) {
  if (this->traits_.supports_brightness()) {
    this->transformer_ = make_unique<LightTransitionTransformer>(millis(), length,
                                                                 this->get_current_values_lazy(),
                                                                 target);
  } else {
    this->set_immediately(target);
  }
  this->send_values();
}

void LightState::set_send_callback(light_send_callback_t send_callback) {
  this->send_callback_ = std::move(send_callback);
}

void LightState::start_flash(const LightColorValues &target, uint32_t length) {
  if (length <= 0)
    return;

  LightColorValues end_colors = this->values_;
  if (this->transformer_ != nullptr)
    end_colors = this->transformer_->get_end_values();
  this->transformer_ = make_unique<LightFlashTransformer>(millis(), length, end_colors, target);
  this->send_values();
}

LightState::LightState(const LightTraits &traits)
    : traits_(traits), effect_(nullptr), transformer_(nullptr), values_(LightColorValues()) {
  this->effect_ = std::move(NoneLightEffect::create());
}

void LightState::set_immediately(const LightColorValues &target) {
  this->transformer_ = nullptr;
  this->values_ = target;
  this->send_values();
}

LightColorValues LightState::get_current_values() {
  this->effect_->apply_effect(this);

  if (this->transformer_ != nullptr) {
    if (this->transformer_->is_finished()) {
      ESP_LOGD(TAG, "Finished transformer.");
      this->values_ = this->transformer_->get_end_values();
      this->transformer_ = nullptr;
      this->send_values();
    } else {
      this->values_ = this->transformer_->get_values();
    }
  }
  return this->values_;
}

void LightState::send_values() {
  if (this->send_callback_)
    this->send_callback_();
}

LightColorValues LightState::get_remote_values() {
  this->effect_->apply_effect(this);

  LightColorValues out = this->values_;
  if (this->transformer_ != nullptr)
    out = this->transformer_->get_remote_values();

  return out;
}

const LightColorValues &LightState::get_current_values_lazy() {
  return this->values_;
}

const LightTraits &LightState::get_traits() const {
  return this->traits_;
}

std::string LightState::get_effect_name() {
  if (this->effect_)
    return this->effect_->get_name();
  else
    return "None";
}

void LightState::start_effect(const std::string &name) {
  for (const LightEffect::Entry &entry : light_effect_entries) {
    if (!this->traits_.supports_traits(entry.requirements))
      continue;
    if (strcasecmp(name.c_str(), entry.name.c_str()) == 0) {
      this->effect_ = std::move(entry.constructor());
      this->effect_->initialize(this);
      this->send_values();
      break;
    }
  }
}

bool LightState::supports_effects() const {
  for (const LightEffect::Entry &entry : light_effect_entries)
    if (this->get_traits().supports_traits(entry.requirements))
      return true;
  return false;
}
void LightState::set_traits(const LightTraits &traits) {
  this->traits_ = traits;
}
void LightState::set_transformer(std::unique_ptr<LightTransformer> transformer) {
  this->transformer_ = std::move(transformer);
}
void LightState::stop_effect() {
  this->effect_ = std::move(NoneLightEffect::create());
}

} // namespace light

} // namespace esphomelib
