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

#include "esphomelib/application.h"

using namespace esphomelib;

class CustomSensor : public Component, public sensor::Sensor {
 public:
  CustomSensor(const std::string &name) : Sensor(name) {}

  void setup() override {

  }

  void loop() override {

  }

};

void setup() {
  App.set_name("test");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_ota()->start_safe_mode();
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");

  auto *custom_sensor = App.register_component(new CustomSensor("Custom Sensor Example"));
  App.register_sensor(custom_sensor);

  App.setup();
}

void loop() {
  App.loop();
}
