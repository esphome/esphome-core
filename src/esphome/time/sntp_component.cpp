#include "esphome/defines.h"

#ifdef USE_SNTP_COMPONENT

#include "esphome/log.h"
#include "esphome/helpers.h"
#include "esphome/component.h"
#include "esphome/time/sntp_component.h"

#ifdef ARDUINO_ARCH_ESP32
#include "apps/sntp/sntp.h"
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include "sntp.h"
#endif

ESPHOME_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "time.sntp";

SNTPComponent::SNTPComponent() : RealTimeClockComponent() {
  this->server_1_ = "0.pool.ntp.org";
  this->server_2_ = "1.pool.ntp.org";
  this->server_3_ = "2.pool.ntp.org";
}
void SNTPComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SNTP...");
#ifdef ARDUINO_ARCH_ESP32
  if (sntp_enabled()) {
    sntp_stop();
  }
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
#endif
#ifdef ARDUINO_ARCH_ESP8266
  sntp_stop();
#endif

  sntp_setservername(0, strdup(this->server_1_.c_str()));
  if (!this->server_2_.empty()) {
    sntp_setservername(1, strdup(this->server_2_.c_str()));
  }
  if (!this->server_3_.empty()) {
    sntp_setservername(2, strdup(this->server_3_.c_str()));
  }

#ifdef ARDUINO_ARCH_ESP8266
  // let localtime/gmtime handle timezones, not sntp
  sntp_set_timezone(0);
#endif
  sntp_init();
}
void SNTPComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "SNTP Time:");
  ESP_LOGCONFIG(TAG, "  Server 1: '%s'", this->server_1_.c_str());
  ESP_LOGCONFIG(TAG, "  Server 2: '%s'", this->server_2_.c_str());
  ESP_LOGCONFIG(TAG, "  Server 3: '%s'", this->server_3_.c_str());
  ESP_LOGCONFIG(TAG, "  Timezone: '%s'", this->timezone_.c_str());
}
void SNTPComponent::set_servers(const std::string &server_1, const std::string &server_2, const std::string &server_3) {
  this->server_1_ = server_1;
  this->server_2_ = server_2;
  this->server_3_ = server_3;
}
float SNTPComponent::get_setup_priority() const { return setup_priority::WIFI; }
void SNTPComponent::loop() {
  if (this->has_time_)
    return;

  auto time = this->now();
  if (!time.is_valid())
    return;

  char buf[128];
  time.strftime(buf, sizeof(buf), "%c");
  ESP_LOGD(TAG, "Synchronized time: %s", buf);
  this->has_time_ = true;
}

}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_SNTP_COMPONENT
