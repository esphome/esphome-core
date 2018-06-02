//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/light/light_state.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"
#include "esphomelib/esphal.h"
#include "esphomelib/light/light_transformer.h"
#include "esphomelib/light/light_effect.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

static const char *TAG = "light.state";

void LightState::start_transition(const LightColorValues &target, uint32_t length) {
  ESP_LOGD(TAG, "Starting transition with length=%u ms", length);

  if (this->get_traits().has_brightness()) {
    this->transformer_ = make_unique<LightTransitionTransformer>(millis(), length,
                                                                 this->get_current_values_lazy(),
                                                                 target);
  } else {
    this->set_immediately(target);
  }
  if (target.get_state() == 0.0f)
    // Turn of effect if transitioning to off.
    this->stop_effect();

  this->send_values();
}

void LightState::add_new_remote_values_callback(light_send_callback_t &&send_callback) {
  this->remote_values_callback_.add(std::move(send_callback));
}

void LightState::start_flash(const LightColorValues &target, uint32_t length) {
  if (length == 0)
    return;
  ESP_LOGD(TAG, "Starting flash with length=%u ms", length);

  LightColorValues end_colors = this->values_;
  if (this->transformer_ != nullptr)
    end_colors = this->transformer_->get_end_values();
  this->transformer_ = make_unique<LightFlashTransformer>(millis(), length, end_colors, target);
  this->send_values();
}

LightState::LightState(const std::string &name, LightOutput *output)
  : Nameable(name), output_(output) {
  this->effect_ = std::move(NoneLightEffect::create());
}

void LightState::set_immediately_without_sending(const LightColorValues &target) {
  this->transformer_ = nullptr;
  this->values_ = target;
}

void LightState::set_immediately(const LightColorValues &target) {
  this->set_immediately_without_sending(target);
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
  this->remote_values_callback_.call();
  this->next_write_ = true;
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

std::string LightState::get_effect_name() {
  if (this->effect_)
    return this->effect_->get_name();
  else
    return "None";
}

void LightState::start_effect(const std::string &name) {
  for (const LightEffect::Entry &entry : light_effect_entries) {
    if (!this->get_traits().supports_traits(entry.requirements))
      continue;
    if (strcasecmp(name.c_str(), entry.name.c_str()) == 0) {
      ESP_LOGD(TAG, "Starting effect '%s'", name.c_str());
      this->effect_->stop(this);
      this->effect_ = std::move(entry.constructor());
      this->effect_->initialize(this);
      this->send_values();
      break;
    }
  }
}

bool LightState::supports_effects() {
  bool found_none_effect = false;
  for (const LightEffect::Entry &entry : light_effect_entries)
    if (this->get_traits().supports_traits(entry.requirements)) {
      if (found_none_effect) {
        return true;
      } else {
        found_none_effect = true;
      }
    }
  return false;
}
void LightState::set_transformer(std::unique_ptr<LightTransformer> transformer) {
  this->transformer_ = std::move(transformer);
}
void LightState::stop_effect() {
  this->effect_->stop(this);
  this->effect_ = std::move(NoneLightEffect::create());
}
void LightState::parse_json(const JsonObject &root) {
  ESP_LOGV(TAG, "Interpreting light JSON.");
  LightColorValues v = this->get_remote_values(); // use remote values for fallback
  v.parse_json(root);
  v.normalize_color(this->output_->get_traits());

  if (root.containsKey("flash")) {
    auto length = uint32_t(float(root["flash"]) * 1000);
    this->start_flash(v, length);
  } else if (root.containsKey("transition")) {
    auto length = uint32_t(float(root["transition"]) * 1000);
    this->start_transition(v, length);
  } else if (root.containsKey("effect")) {
    const char *effect = root["effect"];
    this->start_effect(effect);
  } else {
    this->start_default_transition(v);
  }
}

void LightState::set_default_transition_length(uint32_t default_transition_length) {
  this->default_transition_length_ = default_transition_length;
}
uint32_t LightState::get_default_transition_length() const {
  return this->default_transition_length_;
}
void LightState::dump_json(JsonBuffer &buffer, JsonObject &root) {
  if (this->supports_effects())
    root["effect"] = this->get_effect_name();
  this->get_remote_values().dump_json(root, this->output_->get_traits());
}
void LightState::start_default_transition(const LightColorValues &target) {
  this->start_transition(target, this->default_transition_length_);
}
void LightState::setup() {
  ESP_LOGCONFIG(TAG, "Setting up light '%s'...", this->get_name().c_str());
  LightColorValues recovered_values;
  recovered_values.load_from_preferences(this->get_name());
  this->set_immediately(recovered_values);
}
float LightState::get_setup_priority() const {
  return setup_priority::HARDWARE - 1.0f;
}
LightOutput *LightState::get_output() const {
  return this->output_;
}
float LightState::get_gamma_correct() const {
  return this->gamma_correct_;
}
void LightState::set_gamma_correct(float gamma_correct) {
  this->gamma_correct_ = gamma_correct;
}
void LightState::current_values_as_binary(bool *binary) {
  this->get_current_values().as_binary(binary);
}
void LightState::current_values_as_brightness(float *brightness) {
  this->get_current_values().as_brightness(brightness);
  *brightness = gamma_correct(*brightness, this->gamma_correct_);
}
void LightState::current_values_as_rgb(float *red, float *green, float *blue) {
  this->get_current_values().as_rgb(red, green, blue);
  *red = gamma_correct(*red, this->gamma_correct_);
  *green = gamma_correct(*green, this->gamma_correct_);
  *blue = gamma_correct(*blue, this->gamma_correct_);
}
void LightState::current_values_as_rgbw(float *red, float *green, float *blue, float *white) {
  this->get_current_values().as_rgbw(red, green, blue, white);
  *red = gamma_correct(*red, this->gamma_correct_);
  *green = gamma_correct(*green, this->gamma_correct_);
  *blue = gamma_correct(*blue, this->gamma_correct_);
  *white = gamma_correct(*white, this->gamma_correct_);
}
void LightState::loop() {
  this->effect_->apply_effect(this);

  if (this->next_write_ || (this->transformer_ != nullptr && this->transformer_->is_continuous())) {
    this->output_->write_state(this);
    this->next_write_ = false;
  }
}
LightTraits LightState::get_traits() {
  return this->output_->get_traits();
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
