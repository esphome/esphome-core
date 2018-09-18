//
//  neopixelbus.cpp
//  esphomelib
//
//  Created by Patrick Huy on 07.09.18.
//  Copyright © 2018 Patrick Huy. Some rights reserved.
//

#include <esphomelib.h>

using namespace esphomelib;

uint16_t PixelCount = 150;
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
  neopixel.state->set_default_transition_length(1500);

  App.setup();
}

void loop() {
  App.loop();
  delay(16);
}
