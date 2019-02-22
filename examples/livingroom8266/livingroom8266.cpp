#include <esphome.h>

using namespace esphome;

void setup() {
  App.set_name("livingroom");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto *lamp = App.make_gpio_output(32); // on pin 32
  App.make_binary_light("Standing Lamp", lamp);

  App.make_dht_sensor("Livingroom Temperature", "Livingroom Humidity", 4);

  App.make_monochromatic_light("Desk Lamp", App.make_esp8266_pwm_output(D2));

  App.setup();
}

void loop() {
  App.loop();
}
