#ifndef ESPHOMELIB_API_SUBSCRIBE_LOGS_H
#define ESPHOMELIB_API_SUBSCRIBE_LOGS_H

#include "esphomelib/defines.h"

#ifdef USE_API

#include "esphomelib/component.h"
#include "esphomelib/api/api_message.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace api {

class SubscribeLogsRequest : public APIMessage {
 public:
  bool decode_varint(uint32_t field_id, uint32_t value) override;
  APIMessageType message_type() const override;
  uint32_t get_level() const;
  void set_level(uint32_t level);
 protected:
  uint32_t level_{6};
};

} // namespace api

ESPHOMELIB_NAMESPACE_END

#endif //USE_API

#endif //ESPHOMELIB_API_SUBSCRIBE_LOGS_H
