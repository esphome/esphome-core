#include "esphome/defines.h"

#ifdef USE_TIME

#include "esphome/log.h"
#include "esphome/time/rtc_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "time.rtc";

RealTimeClockComponent::RealTimeClockComponent() {}
void RealTimeClockComponent::set_timezone(const std::string &tz) { this->timezone_ = tz; }
std::string RealTimeClockComponent::get_timezone() { return this->timezone_; }
ESPTime RealTimeClockComponent::now() {
  time_t t = ::time(nullptr);
  struct tm *c_tm = ::localtime(&t);
  return ESPTime::from_tm(c_tm, t);
}
ESPTime RealTimeClockComponent::utcnow() {
  time_t t = ::time(nullptr);
  struct tm *c_tm = ::gmtime(&t);
  return ESPTime::from_tm(c_tm, t);
}
CronTrigger *RealTimeClockComponent::make_cron_trigger() { return new CronTrigger(this); }
void RealTimeClockComponent::call_setup() {
  this->setup_internal_();
  setenv("TZ", this->timezone_.c_str(), 1);
  tzset();
  this->setup();
}

size_t ESPTime::strftime(char *buffer, size_t buffer_len, const char *format) {
  struct tm c_tm = this->to_c_tm();
  return ::strftime(buffer, buffer_len, format, &c_tm);
}
ESPTime ESPTime::from_tm(struct tm *c_tm, time_t c_time) {
  return ESPTime{.second = uint8_t(c_tm->tm_sec),
                 .minute = uint8_t(c_tm->tm_min),
                 .hour = uint8_t(c_tm->tm_hour),
                 .day_of_week = uint8_t(c_tm->tm_wday + 1),
                 .day_of_month = uint8_t(c_tm->tm_mday),
                 .day_of_year = uint16_t(c_tm->tm_yday + 1),
                 .month = uint8_t(c_tm->tm_mon + 1),
                 .year = uint16_t(c_tm->tm_year + 1900),
                 .is_dst = bool(c_tm->tm_isdst),
                 .time = c_time};
}
struct tm ESPTime::to_c_tm() {
  struct tm c_tm = tm{.tm_sec = this->second,
                      .tm_min = this->minute,
                      .tm_hour = this->hour,
                      .tm_mday = this->day_of_month,
                      .tm_mon = this->month - 1,
                      .tm_year = this->year - 1900,
                      .tm_wday = this->day_of_week - 1,
                      .tm_yday = this->day_of_year - 1,
                      .tm_isdst = this->is_dst};
  return c_tm;
}
std::string ESPTime::strftime(const std::string &format) {
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
bool ESPTime::is_valid() const { return this->year >= 2018; }

template<typename T> bool increment_time_value(T &current, uint16_t begin, uint16_t end) {
  current++;
  if (current >= end) {
    current = begin;
    return true;
  }
  return false;
}

void ESPTime::increment_second() {
  this->time++;
  if (!increment_time_value(this->second, 0, 60))
    return;

  // second roll-over, increment minute
  if (!increment_time_value(this->minute, 0, 60))
    return;

  // minute roll-over, increment hour
  if (!increment_time_value(this->hour, 0, 24))
    return;

  // hour roll-over, increment day
  increment_time_value(this->day_of_week, 1, 8);

  static const uint8_t DAYS_IN_MONTH[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  uint8_t days_in_month = DAYS_IN_MONTH[this->month];
  if (this->month == 2 && this->year % 4 == 0)
    days_in_month = 29;

  if (increment_time_value(this->day_of_month, 1, days_in_month + 1)) {
    // day of month roll-over, increment month
    increment_time_value(this->month, 1, 13);
  }

  uint16_t days_in_year = (this->year % 4 == 0) ? 366 : 365;
  if (increment_time_value(this->day_of_year, 1, days_in_year + 1)) {
    // day of year roll-over, increment year
    this->year++;
  }
}
bool ESPTime::operator<(ESPTime other) { return this->time < other.time; }
bool ESPTime::operator<=(ESPTime other) { return this->time <= other.time; }
bool ESPTime::operator==(ESPTime other) { return this->time == other.time; }
bool ESPTime::operator>=(ESPTime other) { return this->time >= other.time; }
bool ESPTime::operator>(ESPTime other) { return this->time > other.time; }
bool ESPTime::in_range() const {
  return this->second < 61 && this->minute < 60 && this->hour < 24 && this->day_of_week > 0 && this->day_of_week < 8 &&
         this->day_of_month > 0 && this->day_of_month < 32 && this->day_of_year > 0 && this->day_of_year < 367 &&
         this->month > 0 && this->month < 13;
}

void CronTrigger::add_second(uint8_t second) { this->seconds_[second] = true; }
void CronTrigger::add_minute(uint8_t minute) { this->minutes_[minute] = true; }
void CronTrigger::add_hour(uint8_t hour) { this->hours_[hour] = true; }
void CronTrigger::add_day_of_month(uint8_t day_of_month) { this->days_of_month_[day_of_month] = true; }
void CronTrigger::add_month(uint8_t month) { this->months_[month] = true; }
void CronTrigger::add_day_of_week(uint8_t day_of_week) { this->days_of_week_[day_of_week] = true; }
bool CronTrigger::matches(const ESPTime &time) {
  return time.is_valid() && this->seconds_[time.second] && this->minutes_[time.minute] && this->hours_[time.hour] &&
         this->days_of_month_[time.day_of_month] && this->months_[time.month] && this->days_of_week_[time.day_of_week];
}
void CronTrigger::loop() {
  ESPTime time = this->rtc_->now();
  if (!time.is_valid())
    return;

  if (this->last_check_.has_value()) {
    if (*this->last_check_ >= time) {
      // already handled this one
      return;
    }

    while (true) {
      this->last_check_->increment_second();
      if (*this->last_check_ >= time)
        break;

      if (this->matches(*this->last_check_))
        this->trigger();
    }
  }

  this->last_check_ = time;
  if (!time.in_range()) {
    ESP_LOGW(TAG, "Time is out of range!");
    ESP_LOGD(TAG, "Second=%02u Minute=%02u Hour=%02u DayOfWeek=%u DayOfMonth=%u DayOfYear=%u Month=%u time=%ld",
             time.second, time.minute, time.hour, time.day_of_week, time.day_of_month, time.day_of_year, time.month,
             time.time);
  }

  if (this->matches(time))
    this->trigger();
}
CronTrigger::CronTrigger(RealTimeClockComponent *rtc) : rtc_(rtc) {}
void CronTrigger::add_seconds(const std::vector<uint8_t> &seconds) {
  for (uint8_t it : seconds)
    this->add_second(it);
}
void CronTrigger::add_minutes(const std::vector<uint8_t> &minutes) {
  for (uint8_t it : minutes)
    this->add_minute(it);
}
void CronTrigger::add_hours(const std::vector<uint8_t> &hours) {
  for (uint8_t it : hours)
    this->add_hour(it);
}
void CronTrigger::add_days_of_month(const std::vector<uint8_t> &days_of_month) {
  for (uint8_t it : days_of_month)
    this->add_day_of_month(it);
}
void CronTrigger::add_months(const std::vector<uint8_t> &months) {
  for (uint8_t it : months)
    this->add_month(it);
}
void CronTrigger::add_days_of_week(const std::vector<uint8_t> &days_of_week) {
  for (uint8_t it : days_of_week)
    this->add_day_of_week(it);
}
float CronTrigger::get_setup_priority() const { return setup_priority::HARDWARE; }

}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_TIME
