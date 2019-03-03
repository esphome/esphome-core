#ifndef ESPHOME_API_SUBSCRIBE_LOGS_H
#define ESPHOME_API_SUBSCRIBE_LOGS_H

#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/component.h"
#include "esphome/api/api_message.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

class SubscribeLogsRequest : public APIMessage {
 public:
  bool decode_varint(uint32_t field_id, uint32_t value) override;
  APIMessageType message_type() const override;
  uint32_t get_level() const;
  void set_level(uint32_t level);
  bool get_dump_config() const;
  void set_dump_config(bool dump_config);

 protected:
  uint32_t level_{6};
  bool dump_config_{false};
};

}  // namespace api

ESPHOME_NAMESPACE_END

#endif  // USE_API

#endif  // ESPHOME_API_SUBSCRIBE_LOGS_H
