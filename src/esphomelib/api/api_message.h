#ifndef ESPHOMELIB_API_MESSAGE_H
#define ESPHOMELIB_API_MESSAGE_H

#include "esphomelib/defines.h"

#ifdef USE_API

#include "esphomelib/component.h"
#include "esphomelib/api/util.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace api {

enum class APIMessageType {
  HELLO_REQUEST = 1,
  HELLO_RESPONSE = 2,
  CONNECT_REQUEST = 3,
  CONNECT_RESPONSE = 4,
  DISCONNECT_REQUEST = 5,
  DISCONNECT_RESPONSE = 6,
  PING_REQUEST = 7,
  PING_RESPONSE = 8,
  DEVICE_INFO_REQUEST = 9,
  DEVICE_INFO_RESPONSE = 10,

  LIST_ENTITIES_REQUEST = 11,
  LIST_ENTITIES_BINARY_SENSOR_RESPONSE = 12,
  LIST_ENTITIES_COVER_RESPONSE = 13,
  LIST_ENTITIES_FAN_RESPONSE = 14,
  LIST_ENTITIES_LIGHT_RESPONSE = 15,
  LIST_ENTITIES_SENSOR_RESPONSE = 16,
  LIST_ENTITIES_SWITCH_RESPONSE = 17,
  LIST_ENTITIES_TEXT_SENSOR_RESPONSE = 18,
  LIST_ENTITIES_DONE_RESPONSE = 19,

  SUBSCRIBE_STATES_REQUEST = 20,
  BINARY_SENSOR_STATE_RESPONSE = 21,
  COVER_STATE_RESPONSE = 22,
  FAN_STATE_RESPONSE = 23,
  LIGHT_STATE_RESPONSE = 24,
  SENSOR_STATE_RESPONSE = 25,
  SWITCH_STATE_RESPONSE = 26,
  TEXT_SENSOR_STATE_RESPONSE = 27,

  SUBSCRIBE_LOGS_REQUEST = 28,
  SUBSCRIBE_LOGS_RESPONSE = 29,

  COVER_COMMAND_REQUEST = 30,
  FAN_COMMAND_REQUEST = 31,
  LIGHT_COMMAND_REQUEST = 32,
  SWITCH_COMMAND_REQUEST = 33,
};

class APIMessage {
 public:
  void decode(uint8_t *buffer, size_t length);
  virtual bool decode_varint(uint32_t field_id, uint32_t value);
  virtual bool decode_length_delimited(uint32_t field_id, const uint8_t *value, size_t len);
  virtual bool decode_32bit(uint32_t field_id, uint32_t value);
  virtual APIMessageType message_type() const = 0;

  virtual void encode(APIBuffer &buffer);
};

} // namespace api

ESPHOMELIB_NAMESPACE_END

#endif //USE_API

#endif //ESPHOMELIB_API_MESSAGE_H
