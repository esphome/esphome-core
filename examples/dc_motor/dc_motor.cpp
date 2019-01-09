#include <esphomelib.h>

using namespace esphomelib;

static const char *TAG = "main";

void setup() {
  App.set_name("dc_motor");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  App.make_h_bridge(22,33);
  ESP_LOGV(TAG, "Humidifier created.");

  App.make_gpio_binary_sensor("Cabinet Motion", 36, "motion");

  App.setup();
}

void loop() {
  App.loop();
}
