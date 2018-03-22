//
// Created by Otto Winter on 22.01.18.
//

#include <esphomelib/application.h>

using namespace esphomelib;

Application app;

void setup() {
  app.set_name("outside");
  app.init_log();

  auto *wifi = app.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  wifi->set_manual_ip(ManualIP{
      .static_ip = IPAddress(192, 168, 178, 42),
      .gateway = IPAddress(192, 168, 178, 1),
      .subnet = IPAddress(255, 255, 255, 0)
  });
  app.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  app.init_ota();

  auto *dallas = app.make_dallas_component(15);

  app.make_mqtt_sensor_for(dallas->get_sensor_by_address(0xfe0000031f1eaf29), "Ambient Temperature");
  app.make_mqtt_sensor_for(dallas->get_sensor_by_address(0x710000031f0e7e28), "Heatpump Temperature");

  app.make_dht_sensor(12, "Outside Temperature", "Outside Humidity");

  app.make_adc_sensor(13, "Analog Voltage");

  app.setup();
}

void loop() {
  app.loop();
}
