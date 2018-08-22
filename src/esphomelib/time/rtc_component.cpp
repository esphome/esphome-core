//
//  rtc_component.cpp
//  esphomelib
//
//  Created by Brad Davidson on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_TIME

#include "esphomelib/log.h"
#include "esphomelib/time/rtc_component.h"
#include "rtc_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "time.rtc";

RTCComponent::RTCComponent(const std::string &tz) {
  setenv("TZ", tz.c_str(), 1);
}
void RTCComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up RTC...");
  tzset();
}
void RTCComponent::set_timezone(const std::string &tz) {
  setenv("TZ", tz.c_str(), 1);
  tzset();
}
std::string RTCComponent::get_timezone() {
  const char *tz = getenv("TZ");
  if (tz == nullptr) {
    return {};
  } else {
    return std::string(tz);
  }
}
EsphomelibTime RTCComponent::now() {
  time_t t = ::time(nullptr);
  struct tm *c_tm = ::localtime(&t);
  return EsphomelibTime::from_tm(c_tm, t);
}
EsphomelibTime RTCComponent::utcnow() {
  time_t t = ::time(nullptr);
  struct tm *c_tm = ::gmtime(&t);
  return EsphomelibTime::from_tm(c_tm, t);
}

size_t EsphomelibTime::strftime(char *buffer, size_t buffer_len, const char *format) {
  struct tm c_tm = this->to_c_tm();
  return ::strftime(buffer, buffer_len, format, &c_tm);
}
EsphomelibTime EsphomelibTime::from_tm(struct tm *c_tm, time_t c_time) {
  return EsphomelibTime{
      .second = uint8_t(c_tm->tm_sec),
      .minute = uint8_t(c_tm->tm_min),
      .hour = uint8_t(c_tm->tm_hour),
      .day_of_week = uint8_t(c_tm->tm_wday + 1),
      .day_of_month = uint8_t(c_tm->tm_mday),
      .day_of_year = uint16_t(c_tm->tm_yday + 1),
      .month = uint8_t(c_tm->tm_mon + 1),
      .year = uint16_t(c_tm->tm_year + 1900),
      .is_dst = bool(c_tm->tm_isdst),
      .time = c_time
  };
}
struct tm EsphomelibTime::to_c_tm() {
  struct tm c_tm = tm{
      .tm_sec = this->second,
      .tm_min = this->minute,
      .tm_hour = this->hour,
      .tm_mday = this->day_of_month,
      .tm_mon = this->month - 1,
      .tm_year = this->year - 1900,
      .tm_wday = this->day_of_week - 1,
      .tm_yday = this->day_of_year - 1,
      .tm_isdst = this->is_dst
  };
  return c_tm;
}
std::string EsphomelibTime::strftime(const std::string &format) {
  std::string timestr;
  timestr.resize(format.size() * 4);
  struct tm c_tm = this->to_c_tm();
  size_t len = ::strftime(&timestr[0], timestr.size(), format.c_str(), &c_tm);
  while (len == 0) {
    timestr.resize(timestr.size() * 2);
    len = ::strftime(&timestr[0], timestr.size(), format.c_str(), &c_tm);
  }
  timestr.resize(len);
  return timestr;
}
bool EsphomelibTime::is_valid() const {
  return this->year >= 2018;
}

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_TIME
