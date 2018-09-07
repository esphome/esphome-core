//
//  neo_pixel_bus_light_output.h
//  esphomelib
//
//  Created by Patrick Huy on 03.09.18.
//  Copyright © 2018 Patrick Huy. Some rights reserved.
//

#ifndef ESPHOMELIB_NEO_PIXEL_BUS_LIGHT_OUTPUT_H
#define ESPHOMELIB_NEO_PIXEL_BUS_LIGHT_OUTPUT_H

#include "esphomelib/power_supply_component.h"
#include "esphomelib/light/light_state.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

// Avoid annoying compiler messages
#include "NeoPixelBrightnessBus.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light
{
/** This component implements support for many types of addressable LED lights.
 *
 * To do this, it uses the NeoPixelBus library. The API for setting up the different
 * types of lights NeoPixelBus supports is intentionally kept as close to NeoPixelBus defaults
 * as possible. To use NeoPixelBus lights with esphomelib, first set up the component using
 * the helper in Application, then add the LEDs using the `add_leds` helper functions.
 *
 * These add_leds helpers can, however, only be called once on a NeoPixelBusLightOutputComponent.
 */
template <typename T_NEO_PIXEL_BUS>
class NeoPixelBusLightOutputComponent : public LightOutput, public Component
{
public:
  /// Only for custom effects: Tell this component to write the new color values on the next loop() iteration.
  void schedule_show()
  {
    ESP_LOGVV(TAG, "Scheduling show...");
    this->next_show_ = true;
  }

  void set_power_supply(PowerSupplyComponent *power_supply)
  {
    this->power_supply_ = power_supply;
  }

  T_NEO_PIXEL_BUS *getcontroller_() const
  {
    return this->controller_;
  }

  /// Set a maximum refresh rate in µs as some lights do not like being updated too often.
  void set_max_refresh_rate(uint32_t interval_us)
  {
    this->max_refresh_rate_ = interval_us;
  }

  /// Add some LEDS, can only be called once.
  T_NEO_PIXEL_BUS &add_leds(T_NEO_PIXEL_BUS *controller)
  {
    assert(this->controller_ == nullptr && "FastLEDLightOutputComponent only supports one controller at a time.");

    this->controller_ = controller;
    for (int i = 0; i < this->controller_->PixelCount(); i++)
      this->controller_->SetPixelColor(i, RgbwColor(0, 0, 0));

    this->controller_->Begin();
    return *this->controller_;
  }

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  LightTraits get_traits() override
  {
    return {true, true, true, true};
  }
  void write_state(LightState *state) override
  {
    float red, green, blue, white, brightness;
    state->current_values_as_rgbw(&red, &green, &blue, &white);
    state->current_values_as_brightness(&brightness);
    uint8_t redb = red * 255;
    uint8_t greenb = green * 255;
    uint8_t blueb = blue * 255;
    uint8_t whiteb = white * 255;
    uint8_t brightnessb = brightness * 255;
    // currently in hass there is no way to only show white via the ui, so disable the colors if all of them are on and therefore very white
    bool white_colors = redb >= brightnessb - 10 && greenb >= brightnessb - 10 && blueb >= brightnessb - 10 && whiteb >= brightnessb - 10;
    if (white_colors) {
       this->controller_->ClearTo(RgbwColor(whiteb));
       
     } else {
      this->controller_->ClearTo(RgbwColor(redb, greenb, blueb, whiteb));
    }

    this->schedule_show();
  }
  void setup() override
  {
    ESP_LOGCONFIG(TAG, "Setting up Neo Pixel Bus light...");
    assert(this->controller_ != nullptr && "You need to add LEDs to this controller!");
    this->controller_->ClearTo(RgbwColor(0, 0, 0));
  }
  void loop() override
  {
    if (!this->next_show_)
      return;

    uint32_t now = micros();
    this->last_refresh_ = now;
    this->next_show_ = false;

    ESP_LOGVV(TAG, "Writing RGB values to bus...");

#ifdef USE_OUTPUT
    if (this->power_supply_ != nullptr)
    {
      bool is_on = false;
      for (int i = 0; i < this->controller_->PixelCount(); i++)
      {
        auto color = this->controller_->GetPixelColor(i);
        if (color.R > 0 || color.G > 0 || color.B > 0 || color.W > 0)
        {
          is_on = true;
          break;
        }
      }

      if (is_on && !this->has_requested_high_power_)
      {
        this->power_supply_->request_high_power();
        this->has_requested_high_power_ = true;
      }
      if (!is_on && this->has_requested_high_power_)
      {
        this->power_supply_->unrequest_high_power();
        this->has_requested_high_power_ = false;
      }
    }
#endif

    this->controller_->Show();
  }
  float get_setup_priority() const override
  {
    return setup_priority::HARDWARE;
  }

protected:
  T_NEO_PIXEL_BUS *controller_{nullptr};
  uint32_t last_refresh_{0};
  bool next_show_{true};
#ifdef USE_OUTPUT
  PowerSupplyComponent *power_supply_{nullptr};
  bool has_requested_high_power_{false};
#endif
private:
  const char *TAG = "light.neo_pixel_bus";
};

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_NEO_PIXEL_BUS_LIGHT

#endif //ESPHOMELIB_LIGHT_NEO_PIXEL_BUS_OUTPUT_H
