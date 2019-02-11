#include <esphome.h>

using namespace esphome;

uint16_t pixel_count = 30;
uint8_t pixel_pin = 2;
// in Neo800KbpsMethod (dma) the pixel_pin is not used and is always PIN 2 (RX)
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(pixel_count, pixel_pin);

void setup() {
  App.set_name("neopixelbus");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto neopixel = App.make_neo_pixel_bus_rgbw_light<Neo800KbpsMethod, NeoGrbwFeature>("NeoPixelBus SK 6812 Light");
  neopixel.output->set_pixel_order(light::ESPNeoPixelOrder::GRBW);
  neopixel.output->add_leds(&strip);
  neopixel.state->set_default_transition_length(800);

  neopixel.state->add_effects({
                                  new light::RandomLightEffect("Random"),
                                  new light::AddressableColorWipeEffect("Color Wipe"),
                                  new light::AddressableRainbowLightEffect("Rainbow"),
                              });

  App.setup();
}

void loop() {
  App.loop();
  delay(16);
}
