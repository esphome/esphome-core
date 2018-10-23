#include <esphomelib.h>

using namespace esphomelib;

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

  auto neopixel = App.make_neo_pixel_bus_light("NeoPixelBus SK 6812 Light", strip);
  neopixel.state->set_default_transition_length(800);

  neopixel.state->add_effects({
    new light::RandomLightEffect("Random"),
    new light::NeoPixelBusLoop<NeoGrbwFeature, Neo800KbpsMethod>("Loop", 60000, 100),
    new light::NeoPixelBusLoop<NeoGrbwFeature, Neo800KbpsMethod>("Short Loop", 20000, 3)
  });

  App.make_partitioned_light("NeoPixelBus Light Partition 1", neopixel.neo_pixel_bus, neopixel.state, 0, 9);
  App.make_partitioned_light("NeoPixelBus Light Partition 2", neopixel.neo_pixel_bus, neopixel.state, 10, 19);
  App.make_partitioned_light("NeoPixelBus Light Partition 3", neopixel.neo_pixel_bus, neopixel.state, 20, 29);

  App.setup();
}

void loop() {
  App.loop();
  delay(16);
}
