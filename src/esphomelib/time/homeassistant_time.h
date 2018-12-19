#ifndef ESPHOMELIB_HOMEASSISTANT_TIME_H
#define ESPHOMELIB_HOMEASSISTANT_TIME_H

#include "esphomelib/defines.h"

#ifdef USE_HOMEASSISTANT_TIME

#include "esphomelib/component.h"
#include "esphomelib/time/rtc_component.h"
#include "esphomelib/api/api_message.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

class HomeAssistantTime : public RealTimeClockComponent {
 public:
  void setup() override;
  void dump_config() override;
  void set_epoch_time(uint32_t epoch);
  float get_setup_priority() const override;
 protected:
};

extern HomeAssistantTime *global_homeassistant_time;

class GetTimeResponse : public api::APIMessage {
 public:
  bool decode_32bit(uint32_t field_id, uint32_t value) override;
  api::APIMessageType message_type() const override;
};

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_HOMEASSISTANT_TIME

#endif //ESPHOMELIB_HOMEASSISTANT_TIME_H
