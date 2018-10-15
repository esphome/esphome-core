#include <esphomelib.h>
#include <esphomelib/light/light_effect.h>

using namespace esphomelib;

/// Custom FastLED effect - Note: this will only work with FastLED lights
class CustomLightEffect : public light::BaseFastLEDLightEffect {
 public:
  CustomLightEffect(const std::string &name) : BaseFastLEDLightEffect(name) {}

  void apply(light::FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override {
    CHSV hsv;
    hsv.val = 255; // brightness
    hsv.sat = 240; // saturation
    hsv.hue = millis() / 70;
    for (CRGB &led : fastled) {
      led = hsv;
      hsv.hue += 10;
    }
    fastled.schedule_show();
  }
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
  fast_led.state->add_effects({new CustomLightEffect("My Custom Effect")});

  App.setup();
}

void loop() {
  App.loop();
  // Make sure this is not too low, some FastLED lights do not like
  // getting updated too often
  delay(16);
}
