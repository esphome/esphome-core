#include "esphome/defines.h"

#ifdef USE_VERSION_TEXT_SENSOR

#include "esphome/text_sensor/version_text_sensor.h"
#include "esphome/log.h"
#include "esphome/util.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.version";

void VersionTextSensor::setup() {
  if (get_app_compilation_time().empty()) {
    this->publish_state(ESPHOME_VERSION);
  } else {
    this->publish_state(ESPHOME_VERSION " " + get_app_compilation_time());
  }
}
float VersionTextSensor::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
VersionTextSensor::VersionTextSensor(const std::string &name) : TextSensor(name) {}

std::string VersionTextSensor::icon() { return "mdi:new-box"; }
std::string VersionTextSensor::unique_id() { return get_mac_address() + "-version"; }
void VersionTextSensor::dump_config() { LOG_TEXT_SENSOR("", "Version Text Sensor", this); }

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_VERSION_TEXT_SENSOR
