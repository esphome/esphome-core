#include "esphome/defines.h"

#ifdef USE_GPS_COMPONENT

#include "esphome/log.h"
#include "esphome/helpers.h"
#include "esphome/component.h"
#include "esphome/time/gps_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace time {

#define SECONDS_FROM_1970_TO_2000 946684800

static const char *TAG = "time.gps";

// utility code, some of this could be exposed i

const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

uint32_t to_unixtime(uint16_t yOff, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss) {
  uint32_t t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000
  return t;
}

GPSComponent::GPSComponent(UARTComponent *parent)
    : RealTimeClockComponent(), UARTDevice(parent) {}

void GPSComponent::get_time_via_uart_(){
    while (this->available() && !this->has_time_)
        if (gps.encode(this->read()))
            if (gps.time.isValid())
            {  
                auto utc_t_ = to_unixtime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
                struct timeval timev {
                .tv_sec = static_cast<time_t>(utc_t_), .tv_usec = 0,
                };
                timezone tz = {0, 0};
                settimeofday(&timev, &tz);
                this->print_datetime();
                this->has_time_ = true;
            }    
}

void GPSComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPS...");
  unsigned long start = millis();
  do 
  {
      this->get_time_via_uart_();
  } while (millis() - start < 1000);
}

void GPSComponent::print_datetime(){
    auto time = this->now();
    char buf[128];
    time.strftime(buf, sizeof(buf), "%c");
    ESP_LOGD(TAG, "Synchronized time: %s", buf);
}

void GPSComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "GPS Time:");
  ESP_LOGCONFIG(TAG, "  Timezone: '%s'", this->timezone_.c_str());
  this->print_datetime();
}

float GPSComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

void GPSComponent::loop() {
    if (this->has_time_)
        return;
    this->get_time_via_uart_();
}
}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_GPS_COMPONENT
