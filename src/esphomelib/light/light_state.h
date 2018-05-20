//
// Created by Otto Winter on 28.11.17.
//

#ifndef ESPHOMELIB_LIGHT_LIGHT_STATE_H
#define ESPHOMELIB_LIGHT_LIGHT_STATE_H

#include <memory>
#include <functional>
#include <vector>
#include "esphomelib/light/light_color_values.h"

#include "esphomelib/light/light_effect.h"
#include "esphomelib/light/light_transformer.h"
#include "esphomelib/component.h"
#include "esphomelib/automation.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

using light_send_callback_t = std::function<void()>;

class LightEffect;
class LightOutput;

template<typename T>
class ToggleAction;
template<typename T>
class TurnOffAction;
template<typename T>
class TurnOnAction;

/** This class represents the communication layer between the front-end MQTT layer and the
 * hardware output layer.
 */
class LightState : public Nameable, public Component {
 public:
  /// Construct this LightState using the provided traits and name.
  LightState(const std::string &name, LightOutput *output);

  /** Start an effect by name. Uses light_effect_entries in light_effect.h for finding the correct effect.
   *
   * @param name The name of the effect, case insensitive.
   */
  void start_effect(const std::string &name);

  /// Stop the current effect (if one is active).
  void stop_effect();

  /** Start a linear transition to the provided target values for a specific amount of time.
   *
   * If this light doesn't support transition (see set_traits()), sets the target values immediately.
   *
   * @param target The target color.
   * @param length The transition length in ms.
   */
  void start_transition(const LightColorValues &target, uint32_t length);

  /** Start a flash for the specified amount of time.
   *
   * Resets to the values that were active when the flash was started after length ms.
   *
   * @param target The target flash color.
   * @param length The flash length in ms.
   */
  void start_flash(const LightColorValues &target, uint32_t length);

  /// Set the color values immediately.
  void set_immediately(const LightColorValues &target);

  /// Set the color values immediately without sending the new state.
  void set_immediately_without_sending(const LightColorValues &target);

  void start_default_transition(const LightColorValues &target);

  void current_values_as_binary(bool *binary);

  void current_values_as_brightness(float *brightness);

  void current_values_as_rgb(float *red, float *green, float *blue);

  void current_values_as_rgbw(float *red, float *green, float *blue, float *white);

  LightTraits get_traits();

  template<typename T>
  ToggleAction<T> *make_toggle_action();

  template<typename T>
  TurnOffAction<T> *make_turn_off_action();

  template<typename T>
  TurnOnAction<T> *make_turn_on_action();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Load state from preferences
  void setup() override;
  void loop() override;
  /// Shortly after HARDWARE.
  float get_setup_priority() const override;

  /// Applies the effect, transformer and then returns the current values.
  LightColorValues get_current_values();

  /// Return the values that should be reported to the remote.
  LightColorValues get_remote_values();

  /// Causes the callback defined by add_send_callback() to trigger.
  void send_values();

  LightOutput *get_output() const;

  /// Manually set a transformer, it's recommended to use start_flash and start_transition instead.
  void set_transformer(std::unique_ptr<LightTransformer> transformer);

  /// Lazily get the last current values. Returns the values last returned by get_current_values().
  const LightColorValues &get_current_values_lazy();

  /// Return the name of the current effect, or if no effect is active "None".
  std::string get_effect_name();

  /** This lets front-end components subscribe to light change events.
   *
   * This is different from add_new_current_values_callback in that it only sends events for start
   * and end values. For example, with transitions it will only send a single callback whereas
   * the callback passed in add_new_current_values_callback will be called every loop() cycle when
   * a transition is active
   *
   * Note the callback should get the output values through get_remote_values().
   *
   * @param send_callback The callback.
   */
  void add_new_remote_values_callback(light_send_callback_t &&send_callback);

  /// Return whether the light has any effects that meet the trait requirements.
  bool supports_effects();

  /// Parse and apply the provided JSON payload.
  void parse_json(const JsonObject &root);

  /// Dump the state of this light as JSON.
  void dump_json(JsonBuffer &buffer, JsonObject &root);

  /// Defaults to 1 second (1000 ms).
  uint32_t get_default_transition_length() const;
  /// Set the default transition length, i.e. the transition length when no transition is provided.
  void set_default_transition_length(uint32_t default_transition_length);

  float get_gamma_correct() const;
  /// Set the gamma correction factor
  void set_gamma_correct(float gamma_correct);

 protected:
  uint32_t default_transition_length_{1000};
  std::unique_ptr<LightEffect> effect_{nullptr};
  std::unique_ptr<LightTransformer> transformer_{nullptr};
  LightColorValues values_{};
  CallbackManager<void()> remote_values_callback_{};
  LightOutput *output_; ///< Store the output to allow effects to have more access.
  bool next_write_{true};
  float gamma_correct_{2.8f};
};

/// Interface to write LightStates to hardware.
class LightOutput {
 public:
  /// Return the LightTraits of this LightOutput.
  virtual LightTraits get_traits() = 0;

  virtual void write_state(LightState *state) = 0;
};

template<typename T>
class ToggleAction : public Action<T> {
 public:
  explicit ToggleAction(LightState *state);

  void set_transition_length(std::function<uint32_t(T)> transition_length);
  void set_transition_length(uint32_t transition_length);

  void play(T x) override;

 protected:
  LightState *state_;
  TemplatableValue<uint32_t, T> transition_length_;
};

template<typename T>
class TurnOffAction : public Action<T> {
 public:
  explicit TurnOffAction(LightState *state);

  void set_transition_length(std::function<uint32_t(T)> transition_length);
  void set_transition_length(uint32_t transition_length);

  void play(T x) override;

 protected:
  LightState *state_;
  TemplatableValue<uint32_t, T> transition_length_;
};

template<typename T>
class TurnOnAction : public Action<T> {
 public:
  explicit TurnOnAction(LightState *state) : state_(state) {}

  void set_transition_length(std::function<uint32_t(T)> &&transition_length);
  void set_transition_length(uint32_t transition_length);
  void set_flash_length(std::function<uint32_t(T)> &&flash_length);
  void set_flash_length(uint32_t flash_length);
  void set_brightness(std::function<float(T)> &&brightness);
  void set_brightness(float brightness);
  void set_red(std::function<float(T)> &&red);
  void set_red(float red);
  void set_green(std::function<float(T)> &&green);
  void set_green(float green);
  void set_blue(std::function<float(T)> &&blue);
  void set_blue(float blue);
  void set_white(std::function<float(T)> &&white);
  void set_white(float white);
  void set_effect(std::function<std::string(T)> &&effect);
  void set_effect(std::string effect);

  void play(T x) override;

 protected:
  LightState *state_;
  TemplatableValue<float, T> brightness_;
  TemplatableValue<float, T> red_;
  TemplatableValue<float, T> green_;
  TemplatableValue<float, T> blue_;
  TemplatableValue<float, T> white_;
  TemplatableValue<uint32_t, T> transition_length_;
  TemplatableValue<uint32_t, T> flash_length_;
  TemplatableValue<std::string, T> effect_;
};

// =============== TEMPLATE DEFINITIONS ===============
template<typename T>
ToggleAction<T> *LightState::make_toggle_action() {
  return new ToggleAction<T>(this);
}
template<typename T>
TurnOffAction<T> *LightState::make_turn_off_action() {
  return new TurnOffAction<T>(this);
}
template<typename T>
TurnOnAction<T> *LightState::make_turn_on_action() {
  return new TurnOnAction<T>(this);
}

template<typename T>
ToggleAction<T>::ToggleAction(LightState *state) : state_(state) {}

template<typename T>
void ToggleAction<T>::play(T x) {
  auto v = this->state_->get_remote_values();
  if (v.get_state() > 0.0f)
    v.set_state(0.0f);
  else
    v.set_state(1.0f);
  if (this->transition_length_.has_value()) {
    this->state_->start_transition(v, this->transition_length_.value(x));
  } else {
    this->state_->start_default_transition(v);
  }
  this->play_next(x);
}
template<typename T>
void ToggleAction<T>::set_transition_length(std::function<uint32_t(T)> transition_length) {
  this->transition_length_ = std::move(transition_length);
}
template<typename T>
void ToggleAction<T>::set_transition_length(uint32_t transition_length) {
  this->transition_length_ = transition_length;
}
template<typename T>
TurnOffAction<T>::TurnOffAction(LightState *state) : state_(state) {}
template<typename T>
void TurnOffAction<T>::play(T x) {
  auto v = this->state_->get_remote_values();
  v.set_state(0.0f);
  if (this->transition_length_.has_value()) {
    this->state_->start_transition(v, this->transition_length_.value(x));
  } else {
    this->state_->start_default_transition(v);
  }
  this->play_next(x);
}
template<typename T>
void TurnOffAction<T>::set_transition_length(std::function<uint32_t(T)> transition_length) {
  this->transition_length_ = std::move(transition_length);
}
template<typename T>
void TurnOffAction<T>::set_transition_length(uint32_t transition_length) {
  this->transition_length_ = transition_length;
}
template<typename T>
void TurnOnAction<T>::play(T x) {
  auto v = this->state_->get_remote_values();
  v.set_state(1.0f);
  if (this->brightness_.has_value()) {
    v.set_brightness(this->brightness_.value(x));
  }
  if (this->red_.has_value()) {
    v.set_red(this->red_.value(x));
  }
  if (this->green_.has_value()) {
    v.set_green(this->green_.value(x));
  }
  if (this->blue_.has_value()) {
    v.set_blue(this->blue_.value(x));
  }
  if (this->white_.has_value()) {
    v.set_white(this->white_.value(x));
  }
  if (this->effect_.has_value()) {
    this->state_->start_effect(this->effect_.value(x));
  } else if (this->flash_length_.has_value()) {
    this->state_->start_flash(v, this->flash_length_.value(x));
  } else if (this->transition_length_.has_value()) {
    this->state_->start_transition(v, this->transition_length_.value(x));
  } else {
    this->state_->start_default_transition(v);
  }
  this->play_next(x);
}
template<typename T>
void TurnOnAction<T>::set_transition_length(std::function<uint32_t(T)> &&transition_length) {
  this->transition_length_ = std::move(transition_length);
}
template<typename T>
void TurnOnAction<T>::set_transition_length(uint32_t transition_length) {
  this->transition_length_ = transition_length;
}
template<typename T>
void TurnOnAction<T>::set_flash_length(std::function<uint32_t(T)> &&flash_length) {
  this->flash_length_ = std::move(flash_length);
}
template<typename T>
void TurnOnAction<T>::set_flash_length(uint32_t flash_length) {
  this->flash_length_ = flash_length;
}
template<typename T>
void TurnOnAction<T>::set_brightness(std::function<float(T)> &&brightness) {
  this->brightness_ = std::move(brightness);
}
template<typename T>
void TurnOnAction<T>::set_brightness(float brightness) {
  this->brightness_ = brightness;
}
template<typename T>
void TurnOnAction<T>::set_red(std::function<float(T)> &&red) {
  this->red_ = std::move(red);
}
template<typename T>
void TurnOnAction<T>::set_red(float red) {
  this->red_ = red;
}
template<typename T>
void TurnOnAction<T>::set_green(std::function<float(T)> &&green) {
  this->green_ = std::move(green);
}
template<typename T>
void TurnOnAction<T>::set_green(float green) {
  this->green_ = green;
}
template<typename T>
void TurnOnAction<T>::set_blue(std::function<float(T)> &&blue) {
  this->blue_ = std::move(blue);
}
template<typename T>
void TurnOnAction<T>::set_blue(float blue) {
  this->blue_ = blue;
}
template<typename T>
void TurnOnAction<T>::set_white(std::function<float(T)> &&white) {
  this->white_ = std::move(white);
}
template<typename T>
void TurnOnAction<T>::set_white(float white) {
  this->white_ = white;
}
template<typename T>
void TurnOnAction<T>::set_effect(std::function<std::string(T)> &&effect) {
  this->effect_ = std::move(effect);
}
template<typename T>
void TurnOnAction<T>::set_effect(std::string effect) {
  this->effect_ = effect;
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_LIGHT_STATE_H
