#include "esphome/defines.h"

#ifdef USE_HOMEASSISTANT_TIME

#include "esphome/time/homeassistant_time.h"
#include "esphome/api/api_server.h"
#include "esphome/log.h"
#include "lwip/opt.h"
#ifdef ARDUINO_ARCH_ESP8266
#include "sys/time.h"
#endif

ESPHOME_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "time.homeassistant";

void HomeAssistantTime::set_epoch_time(uint32_t epoch) {
  struct timeval timev {
    .tv_sec = static_cast<time_t>(epoch), .tv_usec = 0,
  };
  timezone tz = {0, 0};
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
float HomeAssistantTime::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
void HomeAssistantTime::setup() {
  global_homeassistant_time = this;

  this->set_interval(15 * 60 * 1000, [this]() {
    // re-request time every 15 minutes
    api::global_api_server->request_time();
  });
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
api::APIMessageType GetTimeResponse::message_type() const { return api::APIMessageType::GET_TIME_RESPONSE; }

}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_HOMEASSISTANT_TIME
