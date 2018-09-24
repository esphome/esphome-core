//
//  neopixelbus.cpp
//  esphomelib
//
//  Created by Patrick Huy on 07.09.18.
//  Copyright © 2018 Patrick Huy. Some rights reserved.
//

#include <esphomelib.h>
#include <esphomelib/light/neo_pixel_bus_light_effect.h>

using namespace esphomelib;

uint16_t PixelCount = 30;
uint8_t PixelPin = 2;
// in Neo800KbpsMethod (dma) the PixelPin is not used and is always PIN 2 (RX)
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

void setup() {
  App.set_name("neopixelbus");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto neopixel = App.make_neo_pixel_bus_light("NeoPixelBus SK 6812 Light", strip);
  neopixel.state->set_default_transition_length(800);

  auto randomEffect = new light::RandomLightEffect("Random");
  auto loopEffect = new light::NeoPixelBusLoop<NeoGrbwFeature, Neo800KbpsMethod>("Loop", 60000, 100);
  auto shortLoopEffect = new light::NeoPixelBusLoop<NeoGrbwFeature, Neo800KbpsMethod>("Short Loop", 20000, 3);
  std::vector<light::LightEffect*> effects;
  effects.push_back(randomEffect);
  effects.push_back(loopEffect);
  effects.push_back(shortLoopEffect);
  neopixel.state->add_effects(effects);

  App.make_partitioned_light("NeoPixelBus Light Partition 1", neopixel.neo_pixel_bus, neopixel.state, 0, 9);
  App.make_partitioned_light("NeoPixelBus Light Partition 2", neopixel.neo_pixel_bus, neopixel.state, 10, 19);
  App.make_partitioned_light("NeoPixelBus Light Partition 3", neopixel.neo_pixel_bus, neopixel.state, 20, 29);

  App.setup();
}

void loop() {
  App.loop();
  delay(16);
}
