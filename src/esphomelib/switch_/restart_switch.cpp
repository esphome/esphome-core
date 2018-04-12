//
//  restart_switch.cpp
//  esphomelib
//
//  Created by Otto Winter on 29.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/switch_/restart_switch.h"

#include "esphomelib/log.h"

#ifdef USE_RESTART_SWITCH

namespace esphomelib {

namespace switch_ {

static const char *TAG = "switch.restart";

void RestartSwitch::turn_on() {
  ESP_LOGI(TAG, "Restarting device...");
  // first acknowledge command
  this->publish_state(false);
  // then execute
  delay(100); // Let MQTT settle a bit
  ESP.restart();
}
std::string RestartSwitch::icon() {
  return "mdi:restart";
}
void RestartSwitch::turn_off() {
  // Do nothing
  this->publish_state(false);
}

} // namespace switch_

} // namespace esphomelib

#endif //USE_RESTART_SWITCH
