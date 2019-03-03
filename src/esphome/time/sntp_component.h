#ifndef ESPHOME_SNTP_COMPONENT_H
#define ESPHOME_SNTP_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_SNTP_COMPONENT

#include "esphome/component.h"
#include "esphome/time/rtc_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace time {

/// The SNTP component allows you to configure local timekeeping via Simple Network Time Protocol.
///
/// \note
/// The C library (newlib) available on ESPs only supports TZ strings that specify an offset and DST info;
/// you cannot specify zone names or paths to zoneinfo files.
/// \see https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
class SNTPComponent : public RealTimeClockComponent {
 public:
  SNTPComponent();

  void setup() override;
  void dump_config() override;
  /// Change the servers used by SNTP for timekeeping
  void set_servers(const std::string &server_1, const std::string &server_2, const std::string &server_3);
  float get_setup_priority() const override;

  void loop() override;

 protected:
  std::string server_1_;
  std::string server_2_;
  std::string server_3_;
  bool has_time_{false};
};

}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_SNTP_COMPONENT

#endif  // ESPHOME_SNTP_COMPONENT_H
