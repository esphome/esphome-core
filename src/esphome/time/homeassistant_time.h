#ifndef ESPHOME_HOMEASSISTANT_TIME_H
#define ESPHOME_HOMEASSISTANT_TIME_H

#include "esphome/defines.h"

#ifdef USE_HOMEASSISTANT_TIME

#include "esphome/component.h"
#include "esphome/time/rtc_component.h"
#include "esphome/api/api_message.h"

ESPHOME_NAMESPACE_BEGIN

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

}  // namespace time

ESPHOME_NAMESPACE_END

#endif  // USE_HOMEASSISTANT_TIME

#endif  // ESPHOME_HOMEASSISTANT_TIME_H
