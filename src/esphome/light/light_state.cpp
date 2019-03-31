#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/light/light_state.h"

#include "esphome/helpers.h"
#include "esphome/log.h"
#include "esphome/esphal.h"
#include "esphome/light/light_transformer.h"
#include "esphome/light/light_effect.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

static const char *TAG = "light.state";

void LightState::start_transition(const LightColorValues &target, uint32_t length) {
  if (this->get_traits().has_brightness()) {
    this->transformer_ =
        make_unique<LightTransitionTransformer>(millis(), length, this->get_current_values_lazy(), target);
    this->remote_values_ = this->transformer_->get_remote_values();
  } else {
    this->set_immediately(target);
  }
  if (target.get_state() == 0.0f)
    // Turn of effect if transitioning to off.
    this->stop_effect();
}

void LightState::add_new_remote_values_callback(light_send_callback_t &&send_callback) {
  this->remote_values_callback_.add(std::move(send_callback));
}

void LightState::start_flash(const LightColorValues &target, uint32_t length) {
  if (length == 0)
    return;

  LightColorValues end_colors = this->values_;
  if (this->transformer_ != nullptr)
    end_colors = this->transformer_->get_end_values();
  this->transformer_ = make_unique<LightFlashTransformer>(millis(), length, end_colors, target);
}

LightState::LightState(const std::string &name, LightOutput *output) : Nameable(name), output_(output) {}

void LightState::set_immediately_without_sending(const LightColorValues &target) {
  this->transformer_ = nullptr;
  this->values_ = target;
  this->next_write_ = true;
}

void LightState::set_immediately(const LightColorValues &target) {
  this->set_immediately_without_sending(target);
  this->remote_values_ = target;
}

LightColorValues LightState::get_current_values() {
  if (this->transformer_ != nullptr) {
    if (this->transformer_->is_finished()) {
      this->remote_values_ = this->values_ = this->transformer_->get_end_values();
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
  LightColorValues out = this->remote_values_;
  if (this->transformer_ != nullptr)
    out = this->transformer_->get_remote_values();

  return out;
}

const LightColorValues &LightState::get_current_values_lazy() { return this->values_; }

std::string LightState::get_effect_name() {
  if (this->active_effect_ != nullptr)
    return this->active_effect_->get_name();
  else
    return "None";
}

void LightState::start_effect(uint32_t effect_index) {
  this->stop_effect();
  if (effect_index == 0)
    return;

  this->active_effect_ = this->effects_[effect_index - 1];
  this->active_effect_index_ = effect_index;
  this->active_effect_->start_internal();
}

bool LightState::supports_effects() { return !this->effects_.empty(); }
void LightState::set_transformer(std::unique_ptr<LightTransformer> transformer) {
  this->transformer_ = std::move(transformer);
}
void LightState::stop_effect() {
  if (this->active_effect_ != nullptr)
    this->active_effect_->stop();
  this->active_effect_ = nullptr;
  this->active_effect_index_ = 0;
}

void LightState::set_default_transition_length(uint32_t default_transition_length) {
  this->default_transition_length_ = default_transition_length;
}
uint32_t LightState::get_default_transition_length() const { return this->default_transition_length_; }
void LightState::dump_json(JsonObject &root) {
  if (this->supports_effects())
    root["effect"] = this->get_effect_name();
  this->get_remote_values().dump_json(root, this->output_->get_traits());
}

struct LightStateRTCState {
  bool state{false};
  float brightness{1.0f};
  float red{1.0f};
  float green{1.0f};
  float blue{1.0f};
  float white{1.0f};
  float color_temp{1.0f};
  uint32_t effect{0};
};

void LightState::setup() {
  ESP_LOGCONFIG(TAG, "Setting up light '%s'...", this->get_name().c_str());

  this->output_->setup_state(this);
  for (auto *effect : this->effects_) {
    effect->init_internal(this);
  }

  this->rtc_ = global_preferences.make_preference<LightStateRTCState>(this->get_object_id_hash());
  LightStateRTCState recovered{};
  // Attempt to load from preferences, else fall back to default values from struct
  this->rtc_.load(&recovered);

  auto call = this->make_call();
  call.set_state(recovered.state);
  call.set_brightness(recovered.brightness);
  call.set_red(recovered.red);
  call.set_green(recovered.green);
  call.set_blue(recovered.blue);
  call.set_white(recovered.white);
  call.set_color_temperature(recovered.color_temp);
  call.set_effect(recovered.effect);
  call.perform();
}
float LightState::get_setup_priority() const { return setup_priority::HARDWARE - 1.0f; }
LightOutput *LightState::get_output() const { return this->output_; }
float LightState::get_gamma_correct() const { return this->gamma_correct_; }
void LightState::set_gamma_correct(float gamma_correct) { this->gamma_correct_ = gamma_correct; }
void LightState::current_values_as_binary(bool *binary) { this->get_current_values().as_binary(binary); }
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
void LightState::current_values_as_rgbww(float color_temperature_cw, float color_temperature_ww, float *red,
                                         float *green, float *blue, float *cold_white, float *warm_white) {
  this->get_current_values().as_rgbww(color_temperature_cw, color_temperature_ww, red, green, blue, cold_white,
                                      warm_white);
  *red = gamma_correct(*red, this->gamma_correct_);
  *green = gamma_correct(*green, this->gamma_correct_);
  *blue = gamma_correct(*blue, this->gamma_correct_);
  *cold_white = gamma_correct(*cold_white, this->gamma_correct_);
  *warm_white = gamma_correct(*warm_white, this->gamma_correct_);
}
void LightState::current_values_as_cwww(float color_temperature_cw, float color_temperature_ww, float *cold_white,
                                        float *warm_white) {
  this->get_current_values().as_cwww(color_temperature_cw, color_temperature_ww, cold_white, warm_white);
  *cold_white = gamma_correct(*cold_white, this->gamma_correct_);
  *warm_white = gamma_correct(*warm_white, this->gamma_correct_);
}
void LightState::loop() {
  if (this->active_effect_ != nullptr) {
    this->active_effect_->apply();
  }

  if (this->next_write_ || this->transformer_ != nullptr) {
    this->output_->write_state(this);
    this->next_write_ = false;
  }
}
LightTraits LightState::get_traits() { return this->output_->get_traits(); }
const std::vector<LightEffect *> &LightState::get_effects() const { return this->effects_; }
void LightState::add_effects(const std::vector<LightEffect *> effects) {
  this->effects_.reserve(this->effects_.size() + effects.size());
  for (auto *effect : effects) {
    this->effects_.push_back(effect);
  }
}
LightState::StateCall LightState::turn_on() { return this->make_call().set_state(true); }
LightState::StateCall LightState::turn_off() { return this->make_call().set_state(false); }
LightState::StateCall LightState::toggle() {
  return this->make_call().set_state(this->get_remote_values().get_state() == 0.0f);
}
LightState::StateCall LightState::make_call() { return LightState::StateCall(this); }
uint32_t LightState::hash_base() { return 1114400283; }
void LightState::dump_config() {
  ESP_LOGCONFIG(TAG, "Light '%s'", this->get_name().c_str());
  if (this->get_traits().has_brightness()) {
    ESP_LOGCONFIG(TAG, "  Default Transition Length: %u ms", this->default_transition_length_);
    ESP_LOGCONFIG(TAG, "  Gamma Correct: %.2f", this->gamma_correct_);
  }
  if (this->get_traits().has_color_temperature()) {
    ESP_LOGCONFIG(TAG, "  Min Mireds: %.1f", this->get_traits().get_min_mireds());
    ESP_LOGCONFIG(TAG, "  Max Mireds: %.1f", this->get_traits().get_max_mireds());
  }
}
#ifdef USE_MQTT_LIGHT
MQTTJSONLightComponent *LightState::get_mqtt() const { return this->mqtt_; }
void LightState::set_mqtt(MQTTJSONLightComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

LightState::StateCall &LightState::StateCall::set_state(bool state) {
  this->binary_state_ = state;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_state(optional<bool> state) {
  this->binary_state_ = state;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_transition_length(uint32_t transition_length) {
  this->transition_length_ = transition_length;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_transition_length(optional<uint32_t> transition_length) {
  this->transition_length_ = transition_length;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_flash_length(uint32_t flash_length) {
  this->flash_length_ = flash_length;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_flash_length(optional<uint32_t> flash_length) {
  this->flash_length_ = flash_length;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_brightness(float brightness) {
  this->brightness_ = brightness;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_brightness(optional<float> brightness) {
  this->brightness_ = brightness;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_rgb(float red, float green, float blue) {
  this->set_red(red);
  this->set_green(green);
  this->set_blue(blue);
  return *this;
}
LightState::StateCall &LightState::StateCall::set_rgbw(float red, float green, float blue, float white) {
  this->set_rgb(red, green, blue);
  this->set_white(white);
  return *this;
}
LightState::StateCall &LightState::StateCall::set_red(float red) {
  this->red_ = red;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_red(optional<float> red) {
  this->red_ = red;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_green(float green) {
  this->green_ = green;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_green(optional<float> green) {
  this->green_ = green;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_blue(float blue) {
  this->blue_ = blue;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_blue(optional<float> blue) {
  this->blue_ = blue;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_white(float white) {
  this->white_ = white;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_white(optional<float> white) {
  this->white_ = white;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_color_temperature(float color_temperature) {
  this->color_temperature_ = color_temperature;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_color_temperature(optional<float> color_temperature) {
  this->color_temperature_ = color_temperature;
  return *this;
}
LightState::StateCall &LightState::StateCall::set_effect(const std::string &effect) {
  if (strcasecmp(effect.c_str(), "none") == 0) {
    this->effect_ = 0;
    return *this;
  }

  for (uint32_t i = 0; i < this->state_->effects_.size(); i++) {
    LightEffect *e = this->state_->effects_[i];

    if (strcasecmp(effect.c_str(), e->get_name().c_str()) == 0) {
      this->effect_ = i + 1;
      break;
    }
  }
  return *this;
}
LightState::StateCall &LightState::StateCall::set_effect(optional<std::string> effect) {
  if (effect.has_value()) {
    this->set_effect(*effect);
  }
  return *this;
}
LightState::StateCall &LightState::StateCall::set_effect(uint32_t effect_index) {
  this->effect_ = effect_index;
  return *this;
}
LightState::StateCall &LightState::StateCall::parse_color_json(JsonObject &root) {
  if (root.containsKey("state")) {
    auto val = parse_on_off(root["state"]);
    switch (val) {
      case PARSE_ON:
        this->set_state(true);
        break;
      case PARSE_OFF:
        this->set_state(false);
        break;
      case PARSE_TOGGLE:
        this->set_state(this->state_->get_remote_values().get_state() == 0.0f);
        break;
      case PARSE_NONE:
        break;
    }
  }

  if (root.containsKey("brightness")) {
    this->set_brightness(float(root["brightness"]) / 255.0f);
  }

  if (root.containsKey("color")) {
    JsonObject &color = root["color"];
    if (color.containsKey("r") && color.containsKey("g") && color.containsKey("b")) {
      this->set_red(float(color["r"]) / 255.0f);
      this->set_green(float(color["g"]) / 255.0f);
      this->set_blue(float(color["b"]) / 255.0f);
    }
  }

  if (root.containsKey("white_value")) {
    this->set_white(float(root["white_value"]) / 255.0f);
  }

  if (root.containsKey("color_temp")) {
    float color_temp = root["color_temp"];
    this->set_color_temperature(color_temp);
  }

  return *this;
}
LightState::StateCall &LightState::StateCall::parse_json(JsonObject &root) {
  this->parse_color_json(root);

  if (root.containsKey("flash")) {
    auto length = uint32_t(float(root["flash"]) * 1000);
    this->set_flash_length(length);
  }

  if (root.containsKey("transition")) {
    auto length = uint32_t(float(root["transition"]) * 1000);
    this->set_transition_length(length);
  }

  if (root.containsKey("effect")) {
    const char *effect = root["effect"];
    this->set_effect(effect);
  }

  return *this;
}
void LightState::StateCall::perform() const {
  // use remote values for fallback
  LightColorValues v = this->state_->get_remote_values();
  LightTraits traits = this->state_->get_traits();

  if (this->binary_state_.has_value()) {
    const char *state_s = *this->binary_state_ ? "ON" : "OFF";
    ESP_LOGD(TAG, "'%s' Turning %s", this->state_->get_name().c_str(), state_s);
    v.set_state(*this->binary_state_);
  } else {
    ESP_LOGD(TAG, "'%s' Setting", this->state_->get_name().c_str());
  }
  if (traits.has_brightness() && this->brightness_.has_value()) {
    v.set_brightness(*this->brightness_);
    ESP_LOGD(TAG, "  Brightness: %.0f%%", v.get_brightness() * 100.0f);
  }
  if (traits.has_rgb() && this->red_.has_value())
    v.set_red(*this->red_);
  if (traits.has_rgb() && this->green_.has_value())
    v.set_green(*this->green_);
  if (traits.has_rgb() && this->blue_.has_value())
    v.set_blue(*this->blue_);
  if (traits.has_rgb_white_value() && this->white_.has_value())
    v.set_white(*this->white_);

  if (traits.has_color_temperature() && this->color_temperature_.has_value()) {
    v.set_color_temperature(*this->color_temperature_);
    ESP_LOGD(TAG, "  Color Temperature: %.1f mireds", v.get_color_temperature());
  }

  v.normalize_color(traits);

  if (traits.has_rgb() && (this->red_.has_value() || this->green_.has_value() || this->blue_.has_value())) {
    if (traits.has_rgb_white_value() && this->white_.has_value()) {
      ESP_LOGD(TAG, "  Red=%.0f%%, Green=%.0f%%, Blue=%.0f%%, White=%.0f%%", v.get_red() * 100.0f,
               v.get_green() * 100.0f, v.get_blue() * 100.0f, v.get_white() * 100.0f);
    } else {
      ESP_LOGD(TAG, "  Red=%.0f%%, Green=%.0f%%, Blue=%.0f%%", v.get_red() * 100.0f, v.get_green() * 100.0f,
               v.get_blue() * 100.0f);
    }
  }

  if (this->flash_length_.has_value()) {
    ESP_LOGD(TAG, "  Flash Length: %u ms", *this->flash_length_);
    this->state_->start_flash(v, *this->flash_length_);
  } else {
    uint32_t length = this->transition_length_.value_or(this->state_->default_transition_length_);
    if (traits.has_brightness() && length != 0) {
      ESP_LOGD(TAG, "  Transition Length: %u ms", length);
    }
    this->state_->start_transition(v, length);
  }

  if (this->effect_.has_value() && *this->effect_ <= this->state_->effects_.size()) {
    if (*this->effect_ != 0) {
      ESP_LOGD(TAG, "  Effect: '%s'", this->state_->effects_[*this->effect_ - 1]->get_name().c_str());
    }
    this->state_->start_effect(*this->effect_);
  }

  LightStateRTCState saved;
  saved.state = v.get_state() != 0.0f;
  saved.brightness = v.get_brightness();
  saved.red = v.get_red();
  saved.green = v.get_green();
  saved.blue = v.get_blue();
  saved.white = v.get_white();
  saved.color_temp = v.get_color_temperature();
  saved.effect = *this->state_->active_effect_index_;
  if (!this->flash_length_.has_value()) {
    // Do not save RTC state for flashes.
    this->state_->rtc_.save(&saved);
  }
  this->state_->send_values();
}
LightState::StateCall::StateCall(LightState *state) : state_(state) {}

void LightOutput::setup_state(LightState *state) {}

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT
