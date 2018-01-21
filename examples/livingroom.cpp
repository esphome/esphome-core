//
// Created by Otto Winter on 21.01.18.
//

#include <esphomelib/application.h>

using namespace esphomelib;

Application app;

void setup() {
  app.set_name("livingroom");
  app.init_log();

  app.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  app.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  app.init_ota();

  auto *red = app.make_ledc_component(32); // on pin 32
  auto *green = app.make_ledc_component(33);
  auto *blue = app.make_ledc_component(34);
  app.make_rgb_light("Livingroom Light", red, green, blue);

  app.setup();
}

void loop() {
  app.loop();
}
