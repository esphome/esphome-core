#include "esphomelib/defines.h"

#ifdef USE_RESTART_SWITCH

#include "esphomelib/switch_/restart_switch.h"

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.restart";

std::string RestartSwitch::icon() {
  return "mdi:restart";
}
RestartSwitch::RestartSwitch(const std::string &name)
  : Switch(name) {

}
void RestartSwitch::write_state(bool state) {
  // Acknowledge
  this->publish_state(false);

  if (state) {
    ESP_LOGI(TAG, "Restarting device...");
    // then execute
    delay(100); // Let MQTT settle a bit
    safe_reboot("restart");
  }
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_RESTART_SWITCH
