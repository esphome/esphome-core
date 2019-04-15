#ifndef ESPHOME_GPS_COMPONENT_H
#define ESPHOME_GPS_COMPONENT_H

#include "esphome/defines.h"
#include <TinyGPS++.h>
#include "sys/time.h"

#ifdef USE_GPS_COMPONENT

#include "esphome/component.h"
#include "esphome/time/rtc_component.h"
#include "esphome/uart_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace time {

/// The GPS component allows you to configure local timekeeping via Simple Network Time Protocol.
///
/// \note
/// The C library (newlib) available on ESPs only supports TZ strings that specify an offset and DST info;
/// you cannot specify zone names or paths to zoneinfo files.
/// \see https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
class GPSComponent : public RealTimeClockComponent, public UARTDevice  {
 
 public:
  GPSComponent(UARTComponent *parent);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

 protected:
  bool has_time_{false};
  TinyGPSPlus gps;
  void get_time_via_uart_();
  void print_datetime();
};

}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_GPS_COMPONENT

#endif  // ESPHOME_GPS_COMPONENT_H
