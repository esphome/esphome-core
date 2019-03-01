#include "esphome/defines.h"

#ifdef USE_HOMEASSISTANT_TEXT_SENSOR

#include "esphome/text_sensor/homeassistant_text_sensor.h"
#include "esphome/api/api_server.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.homeassistant";

HomeassistantTextSensor::HomeassistantTextSensor(const std::string &name, const std::string &entity_id)
    : TextSensor(name), entity_id_(entity_id) {}
void HomeassistantTextSensor::dump_config() {
  LOG_TEXT_SENSOR("", "Homeassistant Text Sensor", this);
  ESP_LOGCONFIG(TAG, "  Entity ID: '%s'", this->entity_id_.c_str());
}
void HomeassistantTextSensor::setup() {
  api::global_api_server->subscribe_home_assistant_state(this->entity_id_, [this](std::string state) {
    ESP_LOGD(TAG, "'%s': Got state '%s'", this->entity_id_.c_str(), state.c_str());
    this->publish_state(state);
  });
}

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HOMEASSISTANT_TEXT_SENSOR
