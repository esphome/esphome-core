//
//  Created by Lazar Obradovic on 10.10.18.
//  Copyright © 2018 Lazar Obradovic. All rights reserved.
//

#include <esphomelib.h>

using namespace esphomelib;

void setup() {
  App.set_name("mcp23017");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  App.init_i2c(SDA, SCL, true);

  auto *mcp23017 = App.make_mcp23017_component(0x20);
  App.make_gpio_binary_sensor("MCP pin A0 sensor", mcp23017->make_input_pin(0, MCP23017_INPUT_PULLUP));
  App.make_gpio_binary_sensor("MCP pin A1 sensor", mcp23017->make_input_pin(1, MCP23017_INPUT));

  App.make_gpio_switch("MCP pin B0 switch", mcp23017->make_output_pin(8));
  auto *out = App.make_gpio_output(mcp23017->make_output_pin(8));
  App.make_binary_light("MCP pin B0 light", out);

  App.setup();
}

void loop() {
  App.loop();
}
