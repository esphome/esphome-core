#include <esphomelib.h>

using namespace esphomelib;

static const char *TAG = "main";

static const float cold_white_mireds = 1000000 / 6500;  // 6500 K
static const float warm_white_mireds = 1000000 / 2800;  // 2800 K

void setup() {
  App.set_name("sonoff_b1");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  auto *ota = App.init_ota();
  // Set a plaintext password, alternatively use an MD5 hash for maximum security (set_auth_password_hash)
  ota->set_auth_plaintext_password("VERY_SECURE");
  ota->start_safe_mode();

  auto *my9231 = App.make_my9231_component(GPIOOutputPin(12),
                                           GPIOOutputPin(14));
  my9231->set_num_channels(6);  // two MY9231 chips with each 3 channels
  my9231->set_num_chips(2);
  auto *blue = my9231->create_channel(0);
  auto *red = my9231->create_channel(1);
  auto *green = my9231->create_channel(2);
  auto *warm_white = my9231->create_channel(4);
  auto *cold_white = my9231->create_channel(5);
  App.make_rgbww_light("Sonoff B1", cold_white_mireds, warm_white_mireds,
                       red, green, blue, cold_white, warm_white);

  App.setup();
}

void loop() {
  App.loop();
}
