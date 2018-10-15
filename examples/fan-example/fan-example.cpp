#include <esphomelib.h>

using namespace esphomelib;

void setup() {
  App.set_name("livingroom-fan");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto binary_fan = App.make_fan("Binary Fan");
  binary_fan.output->set_binary(App.make_gpio_output(32));

  auto speed_fan = App.make_fan("Speed Fan");
  // 0.0 -> off speed
  // 0.33 -> low speed
  // 0.66 -> medium speed
  // 1.0 -> high speed
  speed_fan.output->set_speed(App.make_ledc_output(33), 0.0, 0.33, 0.66, 1.0);

  auto oscillating_fan = App.make_fan("Oscillating Fan");
  oscillating_fan.output->set_binary(App.make_gpio_output(34));
  oscillating_fan.output->set_oscillation(App.make_gpio_output(35));

  App.setup();
}

void loop() {
  App.loop();
}
