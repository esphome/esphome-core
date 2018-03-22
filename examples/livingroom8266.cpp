//
// Created by Otto Winter on 21.01.18.
//

#include <esphomelib/application.h>

using namespace esphomelib;

void setup() {
  App.set_name("livingroom");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto *lamp = App.make_gpio_output(32); // on pin 32
  App.make_binary_light("Standing Lamp", lamp);

  App.make_dht_sensor(4, "Livingroom Temperature", "Livingroom Humidity");

  App.setup();
}

void loop() {
  App.loop();
}
