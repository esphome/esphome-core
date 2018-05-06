//
//  shutdown_switch.cpp
//  esphomelib
//
//  Created by Otto Winter on 04.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/switch_/shutdown_switch.h"
#include "esphomelib/log.h"

#include <Esp.h>

#ifdef USE_SHUTDOWN_SWITCH

namespace esphomelib {

namespace switch_ {

static const char *TAG = "switch.shutdown";

ShutdownSwitch::ShutdownSwitch(const std::string &name) : Switch(name) {}

void ShutdownSwitch::turn_on() {
  ESP_LOGI(TAG, "Shutting down...");
  // first acknowledge command
  this->publish_state(false);
  // then execute
  delay(100); // Let MQTT settle a bit

  run_safe_shutdown_hooks("shutdown");
#ifdef ARDUINO_ARCH_ESP8266
  ESP.deepSleep(0);
#endif
#ifdef ARDUINO_ARCH_ESP32
  esp_deep_sleep_start();
#endif
}
void ShutdownSwitch::turn_off() {
  // Do nothing
}
std::string ShutdownSwitch::icon() {
  return "mdi:power";
}

} // namespace switch_

} // namespace esphomelib

#endif //USE_SHUTDOWN_SWITCH
