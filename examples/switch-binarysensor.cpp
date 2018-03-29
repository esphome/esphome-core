//
// Created by Otto Winter on 22.01.18.
//

#include <esphomelib/application.h>

using namespace esphomelib;
using namespace esphomelib::switch_::ir;

static const char *TAG = "main";

void setup() {
  App.set_name("ir");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  auto *ir = App.make_ir_transmitter(32);
  App.make_mqtt_switch_for("Panasonic TV On", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25)));
  App.make_mqtt_switch_for("Panasonic TV Off", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD)));
  App.make_mqtt_switch_for("Panasonic TV Mute", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1004C4D)));
  App.make_mqtt_switch_for("Panasonic TV Volume Up", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1000405)));
  App.make_mqtt_switch_for("Panasonic TV Volume Down", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1008485)));
  App.make_mqtt_switch_for("Panasonic TV Program Up", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1002C2D)));
  App.make_mqtt_switch_for("Panasonic TV Program Down", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100ACAD)));

  App.make_gpio_switch(33, "Dehumidifier");
  ESP_LOGV(TAG, "Humidifier created.");

  App.make_gpio_binary_sensor(36, "Cabinet Motion", binary_sensor::device_class::MOTION);

  App.setup();
}

void loop() {
  App.loop();
}
