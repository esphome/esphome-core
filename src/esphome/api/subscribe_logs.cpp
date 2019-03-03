#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/api/subscribe_logs.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

APIMessageType SubscribeLogsRequest::message_type() const { return APIMessageType::SUBSCRIBE_LOGS_REQUEST; }
bool SubscribeLogsRequest::decode_varint(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1:  // LogLevel level = 1;
      this->level_ = value;
      return true;
    case 2:  // bool dump_config = 2;
      this->dump_config_ = value;
      return true;
    default:
      return false;
  }
}
uint32_t SubscribeLogsRequest::get_level() const { return this->level_; }
void SubscribeLogsRequest::set_level(uint32_t level) { this->level_ = level; }
bool SubscribeLogsRequest::get_dump_config() const { return this->dump_config_; }
void SubscribeLogsRequest::set_dump_config(bool dump_config) { this->dump_config_ = dump_config; }
}  // namespace api

ESPHOME_NAMESPACE_END

#endif  // USE_API
