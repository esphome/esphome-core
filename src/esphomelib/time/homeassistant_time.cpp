#include "esphomelib/defines.h"

#ifdef USE_HOMEASSISTANT_TIME

#include "esphomelib/time/homeassistant_time.h"
#include "esphomelib/log.h"
#include "lwip/opt.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "time.homeassistant";

void HomeAssistantTime::set_epoch_time(uint32_t epoch) {
  struct timeval timev {
      .tv_sec = static_cast<time_t>(epoch),
      .tv_usec = 0,
  };
  timezone tz = { 0, 0 };
  settimeofday(&timev, &tz);

  auto time = this->now();
  char buf[128];
  time.strftime(buf, sizeof(buf), "%c");
  ESP_LOGD(TAG, "Synchronized time: %s", buf);
}
void HomeAssistantTime::dump_config() {
  ESP_LOGCONFIG(TAG, "Home Assistant Time:");
  ESP_LOGCONFIG(TAG, "  Timezone: '%s'", this->timezone_.c_str());
}
float HomeAssistantTime::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void HomeAssistantTime::setup() {
  global_homeassistant_time = this;
}

HomeAssistantTime *global_homeassistant_time = nullptr;

bool GetTimeResponse::decode_32bit(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1:
      // fixed32 epoch_seconds = 1;
      if (global_homeassistant_time != nullptr) {
        global_homeassistant_time->set_epoch_time(value);
      }
      return true;
    default:
      return false;
  }
}
api::APIMessageType GetTimeResponse::message_type() const {
  return api::APIMessageType::GET_TIME_RESPONSE;
}

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_HOMEASSISTANT_TIME
