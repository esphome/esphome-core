//
//  lights.cpp
//  esphomelib
//
//  Created by Otto Winter on 21.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/application.h"

using namespace esphomelib;

static const char *TAG = "main";

Application app;

void setup() {
  app.set_name("lights");
  app.init_log();

  app.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  app.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  auto *ota = app.init_ota();
  // Set a plaintext password, alternatively use an MD5 hash for maximum security (set_auth_password_hash)
  ota->set_auth_plaintext_password("VERY_SECURE");

  // Define a power supply, this will automatically be switched on when the RGBW lights require power.
  auto *power_supply = app.make_power_supply(GPIOOutputPin(13, OUTPUT, true)); // on pin 13, output pinMode and inverted.
  // alternatively if you don't want to invert the pin, just
  // auto *power_supply = app.make_power_supply(13);

  Wire.begin(14, 27, 400000);
  auto *pca9685 = app.make_pca9685_component(500.0f);
  auto *red = pca9685->create_channel(0, power_supply, 0.75f); // channel 0, with power supply and 75% power.
  auto *green = pca9685->create_channel(1, power_supply, 1.0f);
  auto *blue = pca9685->create_channel(2, power_supply, 1.0f);
  auto *white = pca9685->create_channel(3, power_supply, 1.0f);
  app.make_rgbw_light("RGBW Lights", red, green, blue, white);

  app.make_binary_light("Desk Lamp", app.make_gpio_binary_output(15));

  app.make_monochromatic_light("Kitchen Lights", app.make_ledc_component(16)); // supports brightness

  app.setup();
}

void loop() {
  app.loop();
}
