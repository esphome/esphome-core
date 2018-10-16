#include <esphomelib.h>

using namespace esphomelib;

static const char *TAG = "main";

void setup() {
  App.set_name("lights");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  auto *ota = App.init_ota();
  // Set a plaintext password, alternatively use an MD5 hash for maximum security (set_auth_password_hash)
  ota->set_auth_plaintext_password("VERY_SECURE");
  ota->start_safe_mode();

  // Define a power supply, this will automatically be switched on when the RGBW lights require power.
  auto *power_supply = App.make_power_supply(GPIOOutputPin(13, OUTPUT, true)); // on pin 13, output pinMode and inverted.
  // alternatively if you don't want to invert the pin, just
  // auto *power_supply = App.make_power_supply(13);

  App.init_i2c(14, 27, 400000); // the last parameter (frequency) is only available on the ESP32.
  auto *pca9685 = App.make_pca9685_component(500.0f);
  auto *red = pca9685->create_channel(0, power_supply, 0.75f); // channel 0, with power supply and 75% power.
  auto *green = pca9685->create_channel(1, power_supply, 1.0f);
  auto *blue = pca9685->create_channel(2, power_supply, 1.0f);
  auto *white = pca9685->create_channel(3, power_supply, 1.0f);
  App.make_rgbw_light("RGBW Lights", red, green, blue, white);

  App.make_binary_light("Desk Lamp", App.make_gpio_output(15));

  // LEDC is only available on ESP32!
  App.make_monochromatic_light("Kitchen Lights", App.make_ledc_output(16)); // supports brightness

  App.setup();
}

void loop() {
  App.loop();
}
