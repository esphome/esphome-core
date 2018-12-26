#include <esphomelib.h>

using namespace esphomelib;

void setup() {
  App.set_name("tx20-sample");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  App.make_tx20_sensor("Windspeed", "Wind Direction Index", "Wind Direction", new GPIOPin(4, INPUT), 500);
}

void loop() {
  App.loop();
}
