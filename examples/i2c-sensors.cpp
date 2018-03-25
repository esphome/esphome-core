//
//  i2c-sensors.cpp
//  esphomelib
//
//  Created by Otto Winter on 25.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

// This class shows you how you can use various i2c sensors with esphomelib

#include "esphomelib/application.h"

using namespace esphomelib;

void setup() {
  App.set_name("outside");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  // This is required to set up the i2c bus. 14 is SDA pin and 27 is SCL pin.
  Wire.begin(14, 27);

  // 0x48 is the default address when address pin is pulled low.
  auto *ads1115 = App.make_ads1115_component(0x48);
  App.make_mqtt_sensor_for(ads1115->get_sensor(ADS1115_MUX_P0_N1, ADS1115_PGA_6P144), "ADS1115 Voltage #1");
  App.make_mqtt_sensor_for(ads1115->get_sensor(ADS1115_MUX_P0_NG, ADS1115_PGA_1P024), "ADS1115 Voltage #2");

  App.setup();
}

void loop() {
  App.loop();
  delay(10); // this keeps the ESP32/ESP8266 from wasting power.
}
