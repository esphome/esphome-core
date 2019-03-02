#ifndef ESPHOME_LIGHT_LIGHT_STATE_H
#define ESPHOME_LIGHT_LIGHT_STATE_H

#include "esphome/defines.h"

#ifdef USE_LIGHT

#include <functional>
#include <vector>
#include "esphome/light/light_color_values.h"
#include "esphome/light/light_effect.h"
#include "esphome/light/light_transformer.h"
#include "esphome/component.h"
#include "esphome/automation.h"
#include "esphome/helpers.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

using light_send_callback_t = std::function<void()>;

class LightEffect;
class LightOutput;

template<typename... Ts> class ToggleAction;
template<typename... Ts> class TurnOffAction;
template<typename... Ts> class TurnOnAction;

#ifdef USE_MQTT_LIGHT
class MQTTJSONLightComponent;
#endif

/** This class represents the communication layer between the front-end MQTT layer and the
 * hardware output layer.
 */
class LightState : public Nameable, public Component {
 public:
  /// Construct this LightState using the provided traits and name.
  LightState(const std::string &name, LightOutput *output);

  /// Start an effect with the given index
  void start_effect(uint32_t effect_index);

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

  void current_values_as_binary(bool *binary);

  void current_values_as_brightness(float *brightness);

  void current_values_as_rgb(float *red, float *green, float *blue);

  void current_values_as_rgbw(float *red, float *green, float *blue, float *white);

  void current_values_as_rgbww(float color_temperature_cw, float color_temperature_ww, float *red, float *green,
                               float *blue, float *cold_white, float *warm_white);

  void current_values_as_cwww(float color_temperature_cw, float color_temperature_ww, float *cold_white,
                              float *warm_white);

  LightTraits get_traits();

  template<typename... Ts> ToggleAction<Ts...> *make_toggle_action();
  template<typename... Ts> TurnOffAction<Ts...> *make_turn_off_action();
  template<typename... Ts> TurnOnAction<Ts...> *make_turn_on_action();

  class StateCall {
   public:
    StateCall(LightState *state);
    LightState::StateCall &set_state(bool state);
    LightState::StateCall &set_state(optional<bool> state);
    LightState::StateCall &set_transition_length(uint32_t transition_length);
    LightState::StateCall &set_transition_length(optional<uint32_t> transition_length);
    LightState::StateCall &set_flash_length(uint32_t flash_length);
    LightState::StateCall &set_flash_length(optional<uint32_t> flash_length);
    LightState::StateCall &set_brightness(float brightness);
    LightState::StateCall &set_brightness(optional<float> brightness);
    LightState::StateCall &set_rgb(float red, float green, float blue);
    LightState::StateCall &set_rgbw(float red, float green, float blue, float white);
    LightState::StateCall &set_red(float red);
    LightState::StateCall &set_red(optional<float> red);
    LightState::StateCall &set_green(float green);
    LightState::StateCall &set_green(optional<float> green);
    LightState::StateCall &set_blue(float blue);
    LightState::StateCall &set_blue(optional<float> blue);
    LightState::StateCall &set_white(float white);
    LightState::StateCall &set_white(optional<float> white);
    LightState::StateCall &set_color_temperature(float color_temperature);
    LightState::StateCall &set_color_temperature(optional<float> color_temperature);
    LightState::StateCall &set_effect(const std::string &effect);
    LightState::StateCall &set_effect(optional<std::string> effect);
    LightState::StateCall &set_effect(uint32_t effect_index);
    LightState::StateCall &parse_color_json(JsonObject &root);
    LightState::StateCall &parse_json(JsonObject &root);

    void perform() const;

   protected:
    LightState *state_;
    optional<bool> binary_state_;
    optional<float> brightness_;
    optional<float> red_;
    optional<float> green_;
    optional<float> blue_;
    optional<float> white_;
    optional<float> color_temperature_;
    optional<uint32_t> transition_length_;
    optional<uint32_t> flash_length_;
    optional<uint32_t> effect_;
  };

  LightState::StateCall turn_on();
  LightState::StateCall turn_off();
  LightState::StateCall toggle();
  LightState::StateCall make_call();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Load state from preferences
  void setup() override;
  void dump_config() override;
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

  /// Dump the state of this light as JSON.
  void dump_json(JsonObject &root);

  /// Defaults to 1 second (1000 ms).
  uint32_t get_default_transition_length() const;
  /// Set the default transition length, i.e. the transition length when no transition is provided.
  void set_default_transition_length(uint32_t default_transition_length);

  float get_gamma_correct() const;
  /// Set the gamma correction factor
  void set_gamma_correct(float gamma_correct);

  const std::vector<LightEffect *> &get_effects() const;

  void add_effects(std::vector<LightEffect *> effects);

#ifdef USE_MQTT_LIGHT
  MQTTJSONLightComponent *get_mqtt() const;
  void set_mqtt(MQTTJSONLightComponent *mqtt);
#endif

 protected:
  uint32_t hash_base() override;

  ESPPreferenceObject rtc_;
  uint32_t default_transition_length_{1000};
  LightEffect *active_effect_{nullptr};
  optional<uint32_t> active_effect_index_{};
  std::unique_ptr<LightTransformer> transformer_{nullptr};
  LightColorValues values_{};
  LightColorValues remote_values_{};
  CallbackManager<void()> remote_values_callback_{};
  LightOutput *output_;  ///< Store the output to allow effects to have more access.
  bool next_write_{true};
  float gamma_correct_{2.8f};
  std::vector<LightEffect *> effects_;
#ifdef USE_MQTT_LIGHT
  MQTTJSONLightComponent *mqtt_{nullptr};
#endif
};

/// Interface to write LightStates to hardware.
class LightOutput {
 public:
  /// Return the LightTraits of this LightOutput.
  virtual LightTraits get_traits() = 0;

  virtual void setup_state(LightState *state);

  virtual void write_state(LightState *state) = 0;
};

template<typename... Ts> class ToggleAction : public Action<Ts...> {
 public:
  explicit ToggleAction(LightState *state);

  template<typename V> void set_transition_length(V value) { this->transition_length_ = value; }

  void play(Ts... x) override;

 protected:
  LightState *state_;
  TemplatableValue<uint32_t, Ts...> transition_length_;
};

template<typename... Ts> class TurnOffAction : public Action<Ts...> {
 public:
  explicit TurnOffAction(LightState *state);

  template<typename V> void set_transition_length(V value) { this->transition_length_ = value; }

  void play(Ts... x) override;

 protected:
  LightState *state_;
  TemplatableValue<uint32_t, Ts...> transition_length_;
};

template<typename... Ts> class TurnOnAction : public Action<Ts...> {
 public:
  explicit TurnOnAction(LightState *state) : state_(state) {}

  template<typename V> void set_transition_length(V value) { this->transition_length_ = value; }
  template<typename V> void set_flash_length(V value) { this->flash_length_ = value; }
  template<typename V> void set_brightness(V value) { this->brightness_ = value; }
  template<typename V> void set_red(V value) { this->red_ = value; }
  template<typename V> void set_green(V value) { this->green_ = value; }
  template<typename V> void set_blue(V value) { this->blue_ = value; }
  template<typename V> void set_white(V value) { this->white_ = value; }
  template<typename V> void set_color_temperature(V value) { this->color_temperature_ = value; }
  template<typename V> void set_effect(V value) { this->effect_ = value; }

  void play(Ts... x) override;

 protected:
  LightState *state_;
  TemplatableValue<float, Ts...> brightness_;
  TemplatableValue<float, Ts...> red_;
  TemplatableValue<float, Ts...> green_;
  TemplatableValue<float, Ts...> blue_;
  TemplatableValue<float, Ts...> white_;
  TemplatableValue<float, Ts...> color_temperature_;
  TemplatableValue<uint32_t, Ts...> transition_length_;
  TemplatableValue<uint32_t, Ts...> flash_length_;
  TemplatableValue<std::string, Ts...> effect_;
};

// =============== TEMPLATE DEFINITIONS ===============
template<typename... Ts> ToggleAction<Ts...> *LightState::make_toggle_action() { return new ToggleAction<Ts...>(this); }
template<typename... Ts> TurnOffAction<Ts...> *LightState::make_turn_off_action() {
  return new TurnOffAction<Ts...>(this);
}
template<typename... Ts> TurnOnAction<Ts...> *LightState::make_turn_on_action() {
  return new TurnOnAction<Ts...>(this);
}

template<typename... Ts> ToggleAction<Ts...>::ToggleAction(LightState *state) : state_(state) {}

template<typename... Ts> void ToggleAction<Ts...>::play(Ts... x) {
  auto call = this->state_->toggle();
  if (this->transition_length_.has_value()) {
    call.set_transition_length(this->transition_length_.value(x...));
  }
  call.perform();
  this->play_next(x...);
}
template<typename... Ts> TurnOffAction<Ts...>::TurnOffAction(LightState *state) : state_(state) {}
template<typename... Ts> void TurnOffAction<Ts...>::play(Ts... x) {
  auto call = this->state_->turn_off();
  if (this->transition_length_.has_value()) {
    call.set_transition_length(this->transition_length_.value(x...));
  }
  call.perform();
  this->play_next(x...);
}
template<typename... Ts> void TurnOnAction<Ts...>::play(Ts... x) {
  auto call = this->state_->turn_on();
  if (this->brightness_.has_value()) {
    call.set_brightness(this->brightness_.value(x...));
  }
  if (this->red_.has_value()) {
    call.set_red(this->red_.value(x...));
  }
  if (this->green_.has_value()) {
    call.set_green(this->green_.value(x...));
  }
  if (this->blue_.has_value()) {
    call.set_blue(this->blue_.value(x...));
  }
  if (this->white_.has_value()) {
    call.set_white(this->white_.value(x...));
  }
  if (this->color_temperature_.has_value()) {
    call.set_color_temperature(this->color_temperature_.value(x...));
  }
  if (this->effect_.has_value()) {
    call.set_effect(this->effect_.value(x...));
  }
  if (this->flash_length_.has_value()) {
    call.set_flash_length(this->flash_length_.value(x...));
  }
  if (this->transition_length_.has_value()) {
    call.set_transition_length(this->transition_length_.value(x...));
  }
  call.perform();
  this->play_next(x...);
}

}  // namespace light

ESPHOME_NAMESPACE_END

#include "esphome/light/mqtt_json_light_component.h"

#endif  // USE_LIGHT

#endif  // ESPHOME_LIGHT_LIGHT_STATE_H
