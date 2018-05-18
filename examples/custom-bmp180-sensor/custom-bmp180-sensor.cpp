//
//  custom_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 23.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

// <WARNING>
//   For this example to compile, you'll first need to install the Adafruit BMP085 library
//   See  for more information
// </WARNING>

#include <esphomelib.h>
#include <Adafruit_BMP085.h>

using namespace esphomelib;

class BMP180Sensor : public sensor::PollingSensorComponent {
 public:
  Adafruit_BMP085 bmp;

  BMP180Sensor(const std::string &friendly_name, uint32_t update_interval)
      : sensor::PollingSensorComponent(friendly_name, update_interval) {}

  void setup() override {
    bmp.begin();
  }

  void update() override {
    int pressure = bmp.readPressure(); // in Pa, or 1/100 hPa
    push_new_value(pressure / 100.0); // convert to hPa
  }

  std::string unit_of_measurement() override { return "hPa"; }
  int8_t accuracy_decimals() override { return 2; } // 2 decimal places of accuracy.
};

void setup() {
  App.set_name("test");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_ota()->start_safe_mode();
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");

  auto *custom_sensor = App.register_component(new BMP180Sensor("Custom Sensor Example", 5000));
  App.register_sensor(custom_sensor);

  App.setup();
}

void loop() {
  App.loop();
}
