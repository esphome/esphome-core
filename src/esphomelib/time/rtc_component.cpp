//
//  rtc.cpp
//  esphomelib
//
//  Created by Brad Davidson on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/log.h"
#include "esphomelib/time/rtc_component.h"

#ifdef USE_RTC_COMPONENT

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "rtc";

EsphomelibTime convert_time_(struct tm *c_tm, time_t c_time){
  return EsphomelibTime {
    .second = c_tm->tm_sec,
    .minute = c_tm->tm_min,
    .hour = c_tm->tm_hour,
    .day_of_week = c_tm->tm_wday + 1,
    .day_of_month = c_tm->tm_mday,
    .day_of_year = c_tm->tm_yday + 1,
    .month = c_tm->tm_mon + 1,
    .year = c_tm->tm_year + 1900,
    .is_dst = c_tm->tm_isdst,
    .time = c_time
  };
} 
struct tm revert_time_(const EsphomelibTime &time){
  struct tm c_tm = tm {
    .tm_sec = time.second,
    .tm_min = time.minute,
    .tm_hour = time.hour,
    .tm_mday = time.day_of_month,
    .tm_mon = time.month - 1,
    .tm_year = time.year - 1900,
    .tm_wday = time.day_of_week - 1,
    .tm_yday = time.day_of_year - 1,
    .tm_isdst = time.is_dst
  };
  return c_tm;
}
RTCComponent::RTCComponent(const std::string &tz){
  setenv("TZ", tz.c_str(), 1);
}
void RTCComponent::setup(){
  ESP_LOGCONFIG(TAG, "Setting up RTC...");
  tzset();
}
void RTCComponent::set_timezone(const std::string &tz){
  setenv("TZ", tz.c_str(), 1);
  tzset();
}
std::string RTCComponent::get_timezone(){
  const char *tz = getenv("TZ");
  if (tz == nullptr){ 
    return {};
  } else {
    return std::string(tz);
  }
}
EsphomelibTime RTCComponent::now(){
  time_t t = ::time(nullptr);
  struct tm *c_tm = ::localtime(&t);
  return convert_time_(c_tm, t);
}
EsphomelibTime RTCComponent::utcnow(){
  time_t t = ::time(nullptr);
  struct tm *c_tm = ::gmtime(&t);
  return convert_time_(c_tm, t);
}
std::string RTCComponent::strftime(const std::string &format, const EsphomelibTime &time){
  assert(format.size() > 0);
  std::string timestr;
  timestr.resize(format.size() * 4);
  struct tm c_tm = revert_time_(time);
  size_t len = ::strftime(&timestr[0], timestr.size(), format.c_str(), &c_tm);
  while (len == 0){
    timestr.resize(timestr.size() * 2);
    len = ::strftime(&timestr[0], timestr.size(), format.c_str(), &c_tm);
  }
  timestr.resize(len);
  return timestr;
}

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_RTC_COMPONENT
