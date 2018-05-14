//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/light/light_effect.h"

#include "esphomelib/helpers.h"
#include "esphomelib/esphal.h"
#include "esphomelib/light/fast_led_light_output.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

std::vector<LightEffect::Entry> light_effect_entries = { // NOLINT
    LightEffect::Entry{
        .name = "None",
        .requirements = LightTraits(),
        .constructor = NoneLightEffect::create
    },
    LightEffect::Entry{
        .name = "Random",
        .requirements = LightTraits(true, true, false),
        .constructor = RandomLightEffect::create
    },
#ifdef USE_FAST_LED_LIGHT
    LightEffect::Entry{
        .name = "Rainbow Effect",
        // This effect requires the Brightness and RGB traits
        .requirements = LightTraits(true, true, false, true),
        .constructor = FastLEDRainbowLightEffect::create
    },
#endif
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

#ifdef USE_FAST_LED_LIGHT
std::unique_ptr<light::LightEffect> FastLEDRainbowLightEffect::create() {
  return make_unique<FastLEDRainbowLightEffect>();
}

std::string FastLEDRainbowLightEffect::get_name() const {
  return "Rainbow Effect";
}

void FastLEDRainbowLightEffect::apply_effect(light::LightState *state) {
  auto *output = (light::FastLEDLightOutputComponent *) state->get_output();
  uint8_t hue = millis() / 75;
  fill_rainbow(output->get_leds(), output->get_num_leds(), hue, 14);

  // make the output show the effect
  output->schedule_show();
}
void FastLEDRainbowLightEffect::initialize(light::LightState *state) {
  // Prevent any normal light set calls (like setting color manually) affecting our LED array
  // while this effect is active. Otherwise, when choosing a color from the front-end while
  // this effect is active, all LEDS would briefly go to the new color but then right back due
  // to this effect

  auto *output = (FastLEDLightOutputComponent *) state->get_output();
  output->prevent_writing_leds();
}
void FastLEDRainbowLightEffect::stop(light::LightState *state) {
  // Tell the light output to respond to normal requests again.

  auto *output = (FastLEDLightOutputComponent *) state->get_output();
  output->unprevent_writing_leds();
}
#endif

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
