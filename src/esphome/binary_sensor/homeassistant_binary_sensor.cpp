#include "esphome/defines.h"

#ifdef USE_HOMEASSISTANT_BINARY_SENSOR

#include "esphome/binary_sensor/homeassistant_binary_sensor.h"
#include "esphome/api/api_server.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor.homeassistant";

void HomeassistantBinarySensor::setup() {
  api::global_api_server->subscribe_home_assistant_state(this->entity_id_, [this](std::string state) {
    auto val = parse_on_off(state.c_str());
    switch (val) {
      case PARSE_NONE:
      case PARSE_TOGGLE:
        ESP_LOGW(TAG, "Can't convert '%s' to binary state!", state.c_str());
        break;
      case PARSE_ON:
        ESP_LOGD(TAG, "'%s': Got state ON", this->entity_id_.c_str());
        this->publish_state(true);
        break;
      case PARSE_OFF:
        ESP_LOGD(TAG, "'%s': Got state OFF", this->entity_id_.c_str());
        this->publish_state(false);
        break;
    }
  });
}
void HomeassistantBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "Homeassistant Binary Sensor", this);
  ESP_LOGCONFIG(TAG, "  Entity ID: '%s'", this->entity_id_.c_str());
}
float HomeassistantBinarySensor::get_setup_priority() const { return setup_priority::WIFI; }
HomeassistantBinarySensor::HomeassistantBinarySensor(const std::string &name, const std::string &entity_id)
    : BinarySensor(name), entity_id_(entity_id) {}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HOMEASSISTANT_BINARY_SENSOR
