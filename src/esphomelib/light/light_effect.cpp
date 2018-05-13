//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/light/light_effect.h"

#include "esphomelib/helpers.h"
#include "esphomelib/esphal.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

std::vector<LightEffect::Entry> light_effect_entries = { // NOLINT
    LightEffect::Entry {
        .name = "None",
        .requirements = LightTraits(),
        .constructor = NoneLightEffect::create
    },
    LightEffect::Entry {
        .name = "Random",
        .requirements = LightTraits(true, true, false),
        .constructor = RandomLightEffect::create
    }
};

void LightEffect::initialize(LightState *state) {

}
void LightEffect::stop(LightState *state) {

}

std::string NoneLightEffect::get_name() const {
  return "None";
}

void NoneLightEffect::apply_effect(LightState *state) {

}

std::unique_ptr<LightEffect> NoneLightEffect::create() {
  return make_unique<NoneLightEffect>();
}

std::string RandomLightEffect::get_name() const {
  return "Random";
}

void RandomLightEffect::apply_effect(LightState *state) {
  uint32_t now = millis();
  if (now - this->last_color_change_ >= 10000) {
    LightColorValues v = state->get_current_values_lazy();

    v.set_state(1.0f);
    if (v.get_brightness() == 0.0f)
      v.set_brightness(1.0f);
    v.set_red(random_float());
    v.set_green(random_float());
    v.set_blue(random_float());
    v.set_white(random_float());
    v.normalize_color(state->get_traits());
    state->start_transition(v, 7000);

    this->last_color_change_ = now;
  }
}

std::unique_ptr<LightEffect> RandomLightEffect::create() {
  return make_unique<RandomLightEffect>();
}

RandomLightEffect::RandomLightEffect()
    : last_color_change_(millis() - 10000) {}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
