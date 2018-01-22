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

  auto *lamp = app.make_gpio_output_component(32); // on pin 32
  app.make_binary_light("Standing Lamp", lamp);

  app.make_dht_component(4, "Livingroom Temperature", "Livingroom Humidity");

  app.setup();
}

void loop() {
  app.loop();
}
