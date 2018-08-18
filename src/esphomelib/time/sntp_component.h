//
//  sntp_component.h
//  esphomelib
//
//  Created by Brad Davidson on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SNTP_COMPONENT_H
#define ESPHOMELIB_SNTP_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/defines.h"
#include "esphomelib/time/rtc_component.h"

#ifdef USE_SNTP_COMPONENT

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

/// The SNTP component allows you to configure local timekeeping via Simple Network Time Protocol.
///
/// \note
/// The C library (newlib) available on ESPs only supports TZ strings that specify an offset and DST info;
/// you cannot specify zone names or paths to zoneinfo files.
/// \see https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
class SNTPComponent : public RTCComponent {
 public:
  SNTPComponent(const std::string &server_1 = "0.pool.ntp.org",
                const std::string &server_2 = "1.pool.ntp.org",
                const std::string &server_3 = "2.pool.ntp.org",
                const std::string &tz = "UTC");
  void setup() override;
  /// Change the servers used by SNTP for timekeeping
  void set_servers(const std::string &server_1,
                   const std::string &server_2,
                   const std::string &server_3);
  float get_setup_priority() const override;
 protected:
  void setup_sntp_();
  std::string server_1_;
  std::string server_2_;
  std::string server_3_;
};

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_SNTP_COMPONENT

#endif //ESPHOMELIB_SNTP_COMPONENT_H
