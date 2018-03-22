//
// Created by Otto Winter on 22.01.18.
//

#include <esphomelib/application.h>

using namespace esphomelib;

void setup() {
  App.set_name("outside");
  App.init_log();

  auto *wifi = App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  wifi->set_manual_ip(ManualIP{
      .static_ip = IPAddress(192, 168, 178, 42),
      .gateway = IPAddress(192, 168, 178, 1),
      .subnet = IPAddress(255, 255, 255, 0)
  });
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  auto *ota = App.init_ota();
  ota->set_auth_plaintext_password("PASSWORD"); // set an optional password
  ota->set_port(3232); // This is the default for ESP32
  ota->set_hostname("custom-hostname"); // manually set the hostname
  ota->start_safe_mode();

  auto *dallas = App.make_dallas_component(15);

  App.make_mqtt_sensor_for(dallas->get_sensor_by_address(0xfe0000031f1eaf29), "Ambient Temperature");
  App.make_mqtt_sensor_for(dallas->get_sensor_by_address(0x710000031f0e7e28), "Heatpump Temperature");

  App.make_dht_sensor(12, "Outside Temperature", "Outside Humidity");

  App.make_adc_sensor(13, "Analog Voltage");

  App.setup();
}

void loop() {
  App.loop();
}
