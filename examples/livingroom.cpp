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

  auto *red = App.make_ledc_output(32); // on pin 32
  auto *green = App.make_ledc_output(33);
  auto *blue = App.make_ledc_output(34);
  App.make_rgb_light("Livingroom Light", red, green, blue);

  App.make_dht_sensor(12, "Livingroom Temperature", "Livingroom Humidity");

  App.setup();
}

void loop() {
  App.loop();
}
