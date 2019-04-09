#include <esphome.h>

using namespace esphome;

void setup() {
  App.set_name("BME680 Test");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  App.init_i2c(21, 22, true);
  App.make_bme680_sensor("BME680 Temperature", "BME680 Pressure", "BME680 Humidity", "BME680 Gas Resistance", "BME680 Air Quality");
  
  App.setup();
}

void loop() {
  App.loop();
}
