#include <esphomelib.h>

using namespace esphomelib;

void setup() {
  App.set_name("pcf8574");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  App.init_i2c(SDA, SCL, true);

  auto *pcf8574 = App.make_pcf8574_component(0x21);
  App.make_gpio_binary_sensor("PCF pin 0 sensor", pcf8574->make_input_pin(0, PCF8574_INPUT));
  App.make_gpio_binary_sensor("PCF pin 0 sensor", 0);
  App.make_gpio_switch("PCF pin 1 switch", pcf8574->make_output_pin(1));
  auto *out = App.make_gpio_output(pcf8574->make_output_pin(2));
  App.make_binary_light("PCF pin 2 light", out);

  App.setup();
}

void loop() {
  App.loop();
}
