//
//  fast_led_light.cpp
//  esphomelib
//
//  Created by Otto Winter on 13.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/application.h"
#include "esphomelib/light/light_effect.h"

using namespace esphomelib;

/// Custom FastLED effect - Note: this will only work with FastLED lights
class CustomLightEffect : public light::LightEffect {
 public:
  static std::unique_ptr<light::LightEffect> create() { return make_unique<CustomLightEffect>(); }

  std::string get_name() const override { return "Custom Rainbow Effect"; }

  void apply_effect(light::LightState *state) override {
    auto *output = (light::FastLEDLightOutputComponent *) state->get_output();
    uint8_t hue = millis() / 20;
    fill_rainbow(output->get_leds(), output->get_num_leds(), hue, 7);

    // make the output show the effect
    output->schedule_show();
  }

  // Prevent any normal light set calls (like setting color manually) affecting our LED array
  // while this effect is active. Otherwise, when choosing a color from the front-end while
  // this effect is active, all LEDS would briefly go to the new color but then right back due
  // to this effect
  void initialize(light::LightState *state) override {
    auto *output = (light::FastLEDLightOutputComponent *) state->get_output();
    output->prevent_writing_leds();
  }
  // Tell the light output to respond to normal requests again.
  void stop(light::LightState *state) override {
    auto *output = (light::FastLEDLightOutputComponent *) state->get_output();
    output->unprevent_writing_leds();
  }
};

// Make an effect entry so that esphomelib can know about it.
// Note that you need to register it too (see setup())
light::LightEffect::Entry custom_light_entry = {
    .name = "Rainbow Effect",
    // This effect requires the Brightness and RGB traits and must be used with FastLED
    .requirements = light::LightTraits(true, true, false, true),
    .constructor = CustomLightEffect::create
};

void setup() {
  App.set_name("fastled");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto fast_led = App.make_fast_led_light("Fast LED Light");
  // 60 NEOPIXEL LEDS on pin GPIO23
  fast_led.fast_led->add_leds<NEOPIXEL, 23>(60);

  // Register our custom effect
  light::light_effect_entries.push_back(custom_light_entry);

  App.setup();
}

void loop() {
  App.loop();
  // Make sure this is not too low, some FastLED lights do not like
  // getting updated too often
  delay(16);
}
