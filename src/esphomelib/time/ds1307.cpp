#include "esphomelib/defines.h"

#ifdef USE_DS1307_TIME

#include "esphomelib/time/ds1307.h"
#include "esphomelib/log.h"
#ifdef ARDUINO_ARCH_ESP8266
  #include "sys/time.h"
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "time.ds1307";

DS1307Time::DS1307Time(I2CComponent *parent, uint8_t address) : I2CDevice(parent, address) { }

void DS1307Time::dump_config() {
  ESP_LOGCONFIG(TAG, "DS1307 Time:");
  LOG_I2C_DEVICE(this);
  if (is_failed()) {
    ESP_LOGE(TAG, "DS1307 failed!");
  }
}

float DS1307Time::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

void DS1307Time::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DS1307...");
  if (!read_data()) {
    mark_failed();
  }
}

bool DS1307Time::read_data() {
  if (!read_bytes(0, ds1307_.raw, sizeof(ds1307_.raw))) {
    ESP_LOGE(TAG, "Can't read I2C data.");
    return false;
  }
  if (ds1307_.reg.ch) {
    ESP_LOGW(TAG, "Clock halted.");
    return false;
  }
  struct tm t {0};
  t.tm_year = ds1307_.reg.year   + 10 * ds1307_.reg.year_10  + 100;
  t.tm_mon  = ds1307_.reg.month  + 10 * ds1307_.reg.month_10 - 1;
  t.tm_mday = ds1307_.reg.day    + 10 * ds1307_.reg.day_10;
  t.tm_hour = ds1307_.reg.hour   + 10 * ds1307_.reg.hour_10;
  t.tm_min  = ds1307_.reg.minute + 10 * ds1307_.reg.minute_10;
  t.tm_sec  = ds1307_.reg.second + 10 * ds1307_.reg.second_10;
  struct timeval tv {::mktime(&t), 0};
  timezone tz = {0, 0};
  ::settimeofday(&tv, &tz);
  time::EsphomelibTime time = now();
  char buf[128];
  time.strftime(buf, sizeof(buf), "%c");
  ESP_LOGD(TAG, "Synchronized time: %s", buf);
  ESP_LOGV(TAG, "Rate select: %0d", ds1307_.reg.rs);
  ESP_LOGV(TAG, "Square-Wave Enable (SQWE): %0d", ds1307_.reg.sqwe);
  ESP_LOGV(TAG, "Output Control (OUT): %0d", ds1307_.reg.out);
  return true;
}

bool DS1307Time::write_data() {
  if (!read_bytes(0, ds1307_.raw, sizeof(ds1307_.raw))) {
    ESP_LOGE(TAG, "Can't read I2C data.");
    return false;
  }
  time::EsphomelibTime time = now();
  if (!time.is_valid()) {
    ESP_LOGE(TAG, "System time is not valid.");
    return false;
  }
  ds1307_.reg.ch        = 0;
  ds1307_.reg.year      = time.year         % 10;
  ds1307_.reg.year_10   = time.year         / 10 % 10;
  ds1307_.reg.month     = time.month        % 10;
  ds1307_.reg.month_10  = time.month        / 10;
  ds1307_.reg.day       = time.day_of_month % 10;
  ds1307_.reg.day_10    = time.day_of_month / 10;
  ds1307_.reg.hour      = time.hour         % 10;
  ds1307_.reg.hour_10   = time.hour         / 10;
  ds1307_.reg.minute    = time.minute       % 10;
  ds1307_.reg.minute_10 = time.minute       / 10;
  ds1307_.reg.second    = time.second       % 10;
  ds1307_.reg.second_10 = time.second       / 10;
  ds1307_.reg.weekday   = time.day_of_week;
  if (!write_bytes(0, ds1307_.raw, sizeof(ds1307_.raw))) {
    ESP_LOGE(TAG, "Can't write I2C data.");
    return false;
  }
  char buf[128];
  time.strftime(buf, sizeof(buf), "%c");
  ESP_LOGD(TAG, "Synchronized time: %s", buf);
  return true;
}

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_DS1307_TIME
