#ifndef ESPHOMELIB_NEO_PIXEL_BUS_LIGHT_OUTPUT_H
#define ESPHOMELIB_NEO_PIXEL_BUS_LIGHT_OUTPUT_H

#include "esphomelib/defines.h"
#include "esphomelib/helpers.h"
#include "esphomelib/light/light_state.h"
#include "esphomelib/log.h"
#include "esphomelib/power_supply_component.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

#include "NeoPixelBus.h"
#include "esphomelib/light/partitioned_light_output.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {
/** This component implements support for many types of addressable LED lights.
 *
 * To do this, it uses the NeoPixelBus library. The API for setting up the different
 * types of lights NeoPixelBus supports is intentionally kept as close to NeoPixelBus defaults
 * as possible. To use NeoPixelBus lights with esphomelib, first set up the component using
 * the helper in Application, then add the LEDs using the `add_leds` helper functions.
 *
 * These add_leds helpers can, however, only be called once on a NeoPixelBusLightOutputComponent.
 */
template <typename T_COLOR_FEATURE, typename T_METHOD>
class NeoPixelBusLightOutputComponent : public PartitionableLightOutput, public Component {
 public:
  /// Only for custom effects: Tell this component to write the new color values on the next loop() iteration.
  void schedule_show() {
    this->next_show_ = true;
  }

  void set_power_supply(PowerSupplyComponent *power_supply);

  NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *getcontroller_() const {
    return this->controller_;
  }

  /// Set a maximum refresh rate in µs as some lights do not like being updated too often.
  void set_max_refresh_rate(uint32_t interval_us) {
    this->max_refresh_rate_ = interval_us;
  }

  /// Only for custom effects: Prevent the LightState from writing over all color values in CRGB.
  void prevent_writing_leds() {
    this->prevent_writing_leds_ = true;
  }

  /// Only for custom effects: Stop prevent_writing_leds. Call this when your effect terminates.
  void unprevent_writing_leds() {
    this->prevent_writing_leds_ = false;
  }

  /// Add some LEDS, can only be called once.
  NeoPixelBus<T_COLOR_FEATURE, T_METHOD> &add_leds(NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller) {
    assert(this->controller_ == nullptr && "NeoPixelBusLightOutputComponent only supports one controller at a time.");

    this->controller_ = controller;
    this->controller_->ClearTo(typename T_COLOR_FEATURE::ColorObject(0, 0, 0));

    this->controller_->Begin();
    return *this->controller_;
  }

  // ========== INTERNAL METHODS ==========
  LightTraits get_traits() override {
    if (std::is_same<typename T_COLOR_FEATURE::ColorObject, RgbwColor>::value) {
      return {true, true, true, true};
    } else {
      return {true, true, false, true};
    }
  }

  void write_state(LightState *state) override {
    if (this->prevent_writing_leds_)
      return;
    this->controller_->ClearTo(
        this->get_light_color<typename T_COLOR_FEATURE::ColorObject>(state, state->get_current_values()));

    const auto remote_values = state->get_remote_values();
    for (auto const &state : this->partitions_states_) {
      state->set_immediately_without_write(remote_values);
    }
    this->schedule_show();
  }

  void write_partition(LightState *state, uint16_t index_start, uint16_t index_end) override {
    if (this->prevent_writing_leds_)
      return;
    this->controller_->ClearTo(
        this->get_light_color<typename T_COLOR_FEATURE::ColorObject>(state, state->get_current_values()), index_start,
        index_end);
    this->schedule_show();
  }

  void setup() override {
    assert(this->controller_ != nullptr && "You need to add LEDs to this controller!");
    this->controller_->ClearTo(typename T_COLOR_FEATURE::ColorObject(0, 0, 0));
  }
  void loop() override {
    if (!this->next_show_)
      return;

    uint32_t now = micros();
    this->last_refresh_ = now;
    this->next_show_ = false;

#ifdef USE_OUTPUT
    if (this->power_supply_ != nullptr) {
      bool is_light_on = false;
      for (int i = 0; i < this->controller_->PixelCount(); i++) {
        auto color = this->controller_->GetPixelColor(i);
        if (is_on(color)) {
          is_light_on = true;
          break;
        }
      }

      if (is_light_on && !this->has_requested_high_power_) {
        this->power_supply_->request_high_power();
        this->has_requested_high_power_ = true;
      }
      if (!is_light_on && this->has_requested_high_power_) {
        this->power_supply_->unrequest_high_power();
        this->has_requested_high_power_ = false;
      }
    }
#endif

    this->controller_->Show();
  }
  float get_setup_priority() const override {
    return setup_priority::HARDWARE;
  }
  template <typename U>
  static typename std::enable_if<std::is_same<U, RgbColor>::value, RgbColor>::type get_light_color(
      LightState *state, const LightColorValues values) {
    float red, green, blue;
    values.as_rgb(&red, &green, &blue);
    red = gamma_correct(red, state->get_gamma_correct());
    green = gamma_correct(green, state->get_gamma_correct());
    blue = gamma_correct(blue, state->get_gamma_correct());
    uint8_t redb = red * 255;
    uint8_t greenb = green * 255;
    uint8_t blueb = blue * 255;
    return RgbColor(redb, greenb, blueb);
  }

  template <typename U>
  static typename std::enable_if<std::is_same<U, RgbwColor>::value, RgbwColor>::type get_light_color(
      LightState *state, const LightColorValues values) {
    float red, green, blue, white, brightness;
    values.as_rgbw(&red, &green, &blue, &white);
    values.as_brightness(&brightness);
    red = gamma_correct(red, state->get_gamma_correct());
    green = gamma_correct(green, state->get_gamma_correct());
    blue = gamma_correct(blue, state->get_gamma_correct());
    white = gamma_correct(white, state->get_gamma_correct());
    uint8_t redb = red * 255;
    uint8_t greenb = green * 255;
    uint8_t blueb = blue * 255;
    uint8_t whiteb = white * 255;
    return RgbwColor(redb, greenb, blueb, whiteb);
  }

 protected:
  NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller_{nullptr};
  uint32_t last_refresh_{0};
  bool next_show_{true};
#ifdef USE_OUTPUT
  PowerSupplyComponent *power_supply_{nullptr};
  bool has_requested_high_power_{false};
#endif
  static bool is_on(const RgbColor &color) {
    return color.R != 0 && color.G != 0 && color.B != 0;
  }

  static bool is_on(const RgbwColor &color) {
    return color.R != 0 && color.G != 0 && color.B != 0 && color.W != 0;
  }

  bool prevent_writing_leds_{false};
};

}  // namespace light

ESPHOMELIB_NAMESPACE_END

#endif  // USE_NEO_PIXEL_BUS_LIGHT

#endif  // ESPHOMELIB_NEO_PIXEL_BUS_LIGHT_OUTPUT_H
