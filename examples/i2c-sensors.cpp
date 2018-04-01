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

  // This is required to set up the i2c bus. 21 is SDA pin and 2 is SCL pin.
  // Alternatively, you can just writ App.init_i2c(); to use the default i2c
  // pins on your board.
  // You only need to call this once.
  App.init_i2c(21, 22);

  // 0x48 is the default address when address pin is pulled low.
  auto *ads1115 = App.make_ads1115_component(0x48);
  App.make_mqtt_sensor_for(ads1115->get_sensor(ADS1115_MUX_P0_N1, ADS1115_PGA_6P144), "ADS1115 Voltage #1");
  App.make_mqtt_sensor_for(ads1115->get_sensor(ADS1115_MUX_P0_NG, ADS1115_PGA_1P024), "ADS1115 Voltage #2");
  App.make_bmp085_sensor("BMP085 Temperature", "BMP085 Pressure");
  App.make_htu21d_sensor("HTU21D Temperature", "HTU21D Humidity");
  App.make_hdc1080_sensor("HDC1080 Temperature", "HDC1080 Humidity");

  App.setup();
}

void loop() {
  App.loop();
  delay(10); // this keeps the ESP32/ESP8266 from wasting power.
}
