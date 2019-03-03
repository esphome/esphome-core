#include "esphome/defines.h"

#ifdef USE_SHUTDOWN_SWITCH

#include "esphome/switch_/shutdown_switch.h"
#include "esphome/log.h"

#include <Esp.h>

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.shutdown";

ShutdownSwitch::ShutdownSwitch(const std::string &name) : Switch(name) {}

std::string ShutdownSwitch::icon() { return "mdi:power"; }
void ShutdownSwitch::write_state(bool state) {
  // Acknowledge
  this->publish_state(false);

  if (state) {
    ESP_LOGI(TAG, "Shutting down...");
    delay(100);  // Let MQTT settle a bit

    run_safe_shutdown_hooks("shutdown");
#ifdef ARDUINO_ARCH_ESP8266
    ESP.deepSleep(0);
#endif
#ifdef ARDUINO_ARCH_ESP32
    esp_deep_sleep_start();
#endif
  }
}
void ShutdownSwitch::dump_config() { LOG_SWITCH("", "Shutdown Switch", this); }

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_SHUTDOWN_SWITCH
