#include "esphomelib/defines.h"

#ifdef USE_HOMEASSISTANT_TEXT_SENSOR

#include "esphomelib/text_sensor/homeassistant_text_sensor.h"
#include "esphomelib/api/api_server.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.homeassistant";

HomeassistantTextSensor::HomeassistantTextSensor(const std::string &name, const std::string &entity_id)
    : TextSensor(name), entity_id_(entity_id) {
  api::global_api_server->subscribe_home_assistant_state(entity_id, [this](std::string state) {
    ESP_LOGD(TAG, "'%s': Got state '%s'", this->entity_id_.c_str(), state.c_str());
    this->publish_state(state);
  });
}

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_HOMEASSISTANT_TEXT_SENSOR
