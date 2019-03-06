#ifndef ESPHOME_TIME_RTC_COMPONENT_H
#define ESPHOME_TIME_RTC_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_TIME

#include "esphome/component.h"
#include "esphome/automation.h"
#include <stdlib.h>
#include <time.h>
#include <bitset>

ESPHOME_NAMESPACE_BEGIN

namespace time {

/// A more user-friendly version of struct tm from time.h
struct ESPTime {
  /** seconds after the minute [0-60]
   * @note second is generally 0-59; the extra range is to accommodate leap seconds.
   */
  uint8_t second;
  /// minutes after the hour [0-59]
  uint8_t minute;
  /// hours since midnight [0-23]
  uint8_t hour;
  /// day of the week; sunday=1 [1-7]
  uint8_t day_of_week;
  /// day of the month [1-31]
  uint8_t day_of_month;
  /// day of the year [1-366]
  uint16_t day_of_year;
  /// month; january=1 [1-12]
  uint8_t month;
  /// year
  uint16_t year;
  /// daylight savings time flag
  bool is_dst;
  /// unix epoch time (seconds since UTC Midnight January 1, 1970)
  time_t time;

  /** Convert this ESPTime struct to a null-terminated c string buffer as specified by the format argument.
   * Up to buffer_len bytes are written.
   *
   * @see https://www.gnu.org/software/libc/manual/html_node/Formatting-Calendar-Time.html#index-strftime
   */
  size_t strftime(char *buffer, size_t buffer_len, const char *format);

  /** Convert this ESPTime struct to a string as specified by the format argument.
   * @see https://www.gnu.org/software/libc/manual/html_node/Formatting-Calendar-Time.html#index-strftime
   *
   * @warning This method uses dynamically allocated strings which can cause heap fragmentation with some
   * microcontrollers.
   */
  std::string strftime(const std::string &format);

  bool is_valid() const;

  bool in_range() const;

  static ESPTime from_tm(struct tm *c_tm, time_t c_time);

  struct tm to_c_tm();

  void increment_second();
  bool operator<(ESPTime other);
  bool operator<=(ESPTime other);
  bool operator==(ESPTime other);
  bool operator>=(ESPTime other);
  bool operator>(ESPTime other);
};

using EsphomelibTime = ESPTime;

class RealTimeClockComponent;

class CronTrigger : public Trigger<>, public Component {
 public:
  explicit CronTrigger(RealTimeClockComponent *rtc);
  void add_second(uint8_t second);
  void add_seconds(const std::vector<uint8_t> &seconds);
  void add_minute(uint8_t minute);
  void add_minutes(const std::vector<uint8_t> &minutes);
  void add_hour(uint8_t hour);
  void add_hours(const std::vector<uint8_t> &hours);
  void add_day_of_month(uint8_t day_of_month);
  void add_days_of_month(const std::vector<uint8_t> &days_of_month);
  void add_month(uint8_t month);
  void add_months(const std::vector<uint8_t> &months);
  void add_day_of_week(uint8_t day_of_week);
  void add_days_of_week(const std::vector<uint8_t> &days_of_week);
  bool matches(const ESPTime &time);
  void loop() override;
  float get_setup_priority() const override;

 protected:
  std::bitset<61> seconds_;
  std::bitset<60> minutes_;
  std::bitset<24> hours_;
  std::bitset<32> days_of_month_;
  std::bitset<13> months_;
  std::bitset<8> days_of_week_;
  RealTimeClockComponent *rtc_;
  optional<ESPTime> last_check_;
};

/// The RealTimeClock class exposes common timekeeping functions via the device's local real-time clock.
///
/// \note
/// The C library (newlib) available on ESPs only supports TZ strings that specify an offset and DST info;
/// you cannot specify zone names or paths to zoneinfo files.
/// \see https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
class RealTimeClockComponent : public Component {
 public:
  explicit RealTimeClockComponent();

  /// Set the time zone.
  void set_timezone(const std::string &tz);

  /// Get the time zone currently in use.
  std::string get_timezone();

  /// Get the time in the currently defined timezone.
  ESPTime now();

  /// Get the time without any time zone or DST corrections.
  ESPTime utcnow();

  CronTrigger *make_cron_trigger();

  void call_setup() override;

 protected:
  std::string timezone_{};
};

}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_TIME

#endif  // ESPHOME_TIME_RTC_COMPONENT_H
