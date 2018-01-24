//
// Created by Otto Winter on 24.01.18.
//

#include <esphomelib/application.h>

using namespace esphomelib;

Application app;

void setup() {
  app.set_name("livingroom-fan");
  app.init_log();

  app.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  app.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  app.init_ota();

  auto binary_fan = app.make_fan("Binary Fan");
  binary_fan.output->set_binary(app.make_gpio_binary_output(32));

  auto speed_fan = app.make_fan("Speed Fan");
  // 0.0 -> off speed
  // 0.33 -> low speed
  // 0.66 -> medium speed
  // 1.0 -> high speed
  speed_fan.output->set_speed(app.make_ledc_component(33), 0.0, 0.33, 0.66, 1.0);

  auto oscillating_fan = app.make_fan("Oscillating Fan");
  oscillating_fan.output->set_binary(app.make_gpio_binary_output(34));
  oscillating_fan.output->set_oscillation(app.make_gpio_binary_output(35));

  app.setup();
}

void loop() {
  app.loop();
}
