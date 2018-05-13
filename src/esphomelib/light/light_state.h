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
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

using light_send_callback_t = std::function<void()>;

class LightEffect;
class LightOutput;

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

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_LIGHT_STATE_H
