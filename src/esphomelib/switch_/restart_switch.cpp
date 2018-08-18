//
//  restart_switch.cpp
//  esphomelib
//
//  Created by Otto Winter on 29.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_RESTART_SWITCH

#include "esphomelib/switch_/restart_switch.h"

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.restart";

void RestartSwitch::turn_on() {
  ESP_LOGI(TAG, "Restarting device...");
  // first acknowledge command
  this->publish_state(false);
  // then execute
  delay(100); // Let MQTT settle a bit
  safe_reboot("restart");
}
std::string RestartSwitch::icon() {
  return "mdi:restart";
}
void RestartSwitch::turn_off() {
  // Do nothing
  this->publish_state(false);
}
RestartSwitch::RestartSwitch(const std::string &name) : Switch(name) {}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_RESTART_SWITCH
