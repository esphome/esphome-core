#include <esphome.h>

using namespace esphome;

void setup() {
  App.set_name("pcf8574");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  App.init_i2c(SDA, SCL, true);

  auto *pcf8574 = App.make_pcf8574_component(0x21);
  App.make_gpio_binary_sensor("PCF pin 0 sensor", pcf8574->make_input_pin(0, INPUT));
  App.make_gpio_binary_sensor("PCF pin 1 sensor", pcf8574->make_input_pin(1, INPUT_PULLUP));

  App.make_gpio_switch("PCF pin 4 switch", pcf8574->make_output_pin(4));
  auto *out = App.make_gpio_output(pcf8574->make_output_pin(5));
  App.make_binary_light("PCF pin 5 light", out);

  App.setup();
}

void loop() {
  App.loop();
}
