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

namespace esphomelib {

namespace light {

using light_send_callback_t = std::function<void()>;

class LightEffect;

/** LightState - This class represents the communication layer between the front-end MQTT layer and the
 * hardware output layer.
 */
class LightState {
 public:
  /// Construct this LightState using the provided traits.
  explicit LightState(const LightTraits &traits);

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

  void set_transformer(std::unique_ptr<LightTransformer> transformer);

  /// Set the color values immediately.
  void set_immediately(const LightColorValues &target);

  /// Applies the effect, transformer and then returns the current values.
  LightColorValues get_current_values();

  /// Lazily get the last current values. Returns the values last returned by get_current_values().
  const LightColorValues &get_current_values_lazy();

  /// Return the values that should be reported to the remote.
  LightColorValues get_remote_values();

  /// Return the name of the current effect, or if no effect is active "None".
  std::string get_effect_name();

  /** This lets front-end components subscribe to light change events, for example when a transition is done.
   *
   * Note the callback should get the output values through get_remote_values().
   *
   * @param send_callback
   */
  void set_send_callback(light_send_callback_t send_callback);

  /// Causes the callback defined by set_send_callback() to trigger.
  void send_values();

  /// Return whether the light has any effects that meet the trait requirements.
  bool supports_effects() const;

  const LightTraits &get_traits() const;
  void set_traits(const LightTraits &traits);

 protected:
  std::unique_ptr<LightEffect> effect_;
  std::unique_ptr<LightTransformer> transformer_;
  LightColorValues values_;
  light_send_callback_t send_callback_;
  LightTraits traits_;
};

} // namespace light

} // namespace esphomelib


#endif //ESPHOMELIB_LIGHT_LIGHT_STATE_H
