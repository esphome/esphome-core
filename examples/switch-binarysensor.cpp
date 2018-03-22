//
// Created by Otto Winter on 22.01.18.
//

#include <esphomelib/application.h>

using namespace esphomelib;
using namespace esphomelib::output::ir;

Application app;

static const char *TAG = "main";

void setup() {
  app.set_name("ir");
  app.init_log();

  app.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  app.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  app.init_ota();

  auto *ir = app.make_ir_transmitter_component(32);
  app.make_mqtt_switch_for("Panasonic TV On", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25)));
  app.make_mqtt_switch_for("Panasonic TV Off", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD)));
  app.make_mqtt_switch_for("Panasonic TV Mute", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1004C4D)));
  app.make_mqtt_switch_for("Panasonic TV Volume Up", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1000405)));
  app.make_mqtt_switch_for("Panasonic TV Volume Down", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1008485)));
  app.make_mqtt_switch_for("Panasonic TV Program Up", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1002C2D)));
  app.make_mqtt_switch_for("Panasonic TV Program Down", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100ACAD)));

  app.make_gpio_switch(33, "Dehumidifier");
  ESP_LOGV(TAG, "Humidifier created.");

  app.make_gpio_binary_sensor(36, "Cabinet Motion", binary_sensor::device_class::MOTION);

  app.setup();
}

void loop() {
  app.loop();
}
