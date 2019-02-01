#ifndef ESPHOMELIB_DS1307_TIME_H
#define ESPHOMELIB_DS1307_TIME_H

#include "esphomelib/defines.h"

#ifdef USE_DS1307_TIME

#include "esphomelib/component.h"
#include "esphomelib/time/rtc_component.h"
#include "esphomelib/i2c_component.h"


ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

class DS1307Time : public RealTimeClockComponent, public I2CDevice {
 public:
  DS1307Time(I2CComponent *parent, uint8_t address);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  bool read_data();
  bool write_data();
 protected:
  union DS1307Reg {
    struct {
      uint8_t second     : 4;
      uint8_t second_10  : 3;
      bool    ch         : 1;
  
      uint8_t minute     : 4;
      uint8_t minute_10  : 3;
      uint8_t unused_1   : 1;
  
      uint8_t hour       : 4;
      uint8_t hour_10    : 2;
      uint8_t unused_2   : 2;
  
      uint8_t weekday    : 3;
      uint8_t unused_3   : 5;
  
      uint8_t day        : 4;
      uint8_t day_10     : 2;
      uint8_t unused_4   : 2;
  
      uint8_t month      : 4;
      uint8_t month_10   : 1;
      uint8_t unused_5   : 3;
  
      uint8_t year       : 4;
      uint8_t year_10    : 4;
  
      uint8_t rs         : 2;
      uint8_t unused_6   : 2;
      bool    sqwe       : 1;
      uint8_t unused_7   : 2;
      bool    out        : 1;
    } reg;
    uint8_t raw[sizeof(reg)];
  } ds1307_;

};

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_DS1307_TIME

#endif //ESPHOMELIB_DS1307_TIME_H
