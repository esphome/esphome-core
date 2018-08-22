//
//  rtc_component.h
//  esphomelib
//
//  Created by Brad Davidson on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_TIME_RTC_COMPONENT_H
#define ESPHOMELIB_TIME_RTC_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/defines.h"
#include <stdlib.h>
#include <time.h>

#ifdef USE_TIME

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

/// A more user-friendly version of struct tm from time.h
struct EsphomelibTime {
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

  /** Convert this EsphomelibTime struct to a null-terminated c string buffer as specified by the format argument.
   * Up to buffer_len bytes are written.
   *
   * @see https://www.gnu.org/software/libc/manual/html_node/Formatting-Calendar-Time.html#index-strftime
   */
  size_t strftime(char *buffer, size_t buffer_len, const char *format);

  /** Convert this EsphomelibTime struct to a string as specified by the format argument.
   * @see https://www.gnu.org/software/libc/manual/html_node/Formatting-Calendar-Time.html#index-strftime
   *
   * @warning This method uses dynamically allocated strings which can cause heap fragmentation with some
   * microcontrollers.
   */
  std::string strftime(const std::string &format);

  bool is_valid() const;

  static EsphomelibTime from_tm(struct tm *c_tm, time_t c_time);

  struct tm to_c_tm();
};

/// The RTC component exposes common timekeeping functions via the device's local real-time clock.
///
/// \note
/// The C library (newlib) available on ESPs only supports TZ strings that specify an offset and DST info;
/// you cannot specify zone names or paths to zoneinfo files.
/// \see https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
class RTCComponent : public Component {
 public:
  explicit RTCComponent(const std::string &tz);

  void setup() override;

  // Set the time zone.
  void set_timezone(const std::string &tz);

  /// Get the time zone currently in use.
  std::string get_timezone();

  /// Get the time in the currently defined timezone.
  EsphomelibTime now();

  /// Get the time without any time zone or DST corrections.
  EsphomelibTime utcnow();
};

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_TIME

#endif //ESPHOMELIB_TIME_RTC_COMPONENT_H
