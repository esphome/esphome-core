#include <esphomelib.h>

using namespace esphomelib;


void setup() {
  App.set_name("dc_motor");
  App.init_log();

  App.init_wifi("FuckGFW", "refuckgfw");
  App.init_mqtt("192.168.123.41", "von", "von1970");
  App.init_ota()->start_safe_mode();

  auto bridge = App.make_h_bridge(5,4);
  bridge->move();
  bridge->set_direction(true);
  bridge->move();
  App.make_gpio_binary_sensor("Cabinet Motion", 16, "motion");

  App.setup();
}

void loop() {
  App.loop();
}
