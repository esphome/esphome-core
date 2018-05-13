//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_LIGHT_LIGHT_EFFECT_H
#define ESPHOMELIB_LIGHT_LIGHT_EFFECT_H

#include <string>

#include "esphomelib/light/light_traits.h"
#include "esphomelib/light/light_state.h"
#include "esphomelib/defines.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

class LightState;

/** Base-class for all light effects.
 *
 * In order to create a custom effect, first create a subclass implementing apply_effect and optionally initialize
 * which both can use the provided LightState to set light colors and/or start transitions. Next, override get_name()
 * and return the name of your effect. Lastly, you need to register your effect in light_effect_entries using the
 * LightEffect::Entry struct.
 */
class LightEffect {
 public:
  /// Internal struct for light_effect_entries.
  struct Entry {
    std::string name; ///< The name of your effect.
    LightTraits requirements; ///< Which traits the light needs to have for this effect.
    std::function<std::unique_ptr<LightEffect>()> constructor; ///< A function creating a your light effect.
  };

  /// Return the name of this effect.
  virtual std::string get_name() const = 0;

  /// Initialize this LightEffect. Will be called once after creation.
  virtual void initialize(LightState *state);

  /// Apply this effect. Use the provided state for starting transitions, ...
  virtual void apply_effect(LightState *state) = 0;
};

/// Default effect for all lights. Does nothing.
class NoneLightEffect : public LightEffect {
 public:
  std::string get_name() const override;

  void apply_effect(LightState *state) override;

  static std::unique_ptr<LightEffect> create();
};

/// Random effect. Sets random colors every 10 seconds and slowly transitions between them.
class RandomLightEffect : public LightEffect {
 public:
  RandomLightEffect();

  std::string get_name() const override;

  void apply_effect(LightState *state) override;

  static std::unique_ptr<LightEffect> create();

 protected:
  uint32_t last_color_change_;
};

extern std::vector<LightEffect::Entry> light_effect_entries;

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_LIGHT_EFFECT_H
