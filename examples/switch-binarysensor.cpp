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
  App.make_mqtt_switch_for(ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25)), "Panasonic TV On");
  App.make_mqtt_switch_for(ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD)), "Panasonic TV Off");
  App.make_mqtt_switch_for(ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1004C4D)), "Panasonic TV Mute");
  App.make_mqtt_switch_for(ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1000405)), "Panasonic TV Volume Up");
  App.make_mqtt_switch_for(ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1008485)), "Panasonic TV Volume Down");
  App.make_mqtt_switch_for(ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1002C2D)), "Panasonic TV Program Up");
  App.make_mqtt_switch_for(ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100ACAD)), "Panasonic TV Program Down");

  App.make_gpio_switch(33, "Dehumidifier");
  ESP_LOGV(TAG, "Humidifier created.");

  App.make_gpio_binary_sensor(36, "Cabinet Motion", "motion");

  App.setup();
}

void loop() {
  App.loop();
}
