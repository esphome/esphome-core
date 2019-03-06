#ifndef ESPHOME_API_BASIC_MESSAGES_H
#define ESPHOME_API_BASIC_MESSAGES_H

#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/api/api_message.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

class HelloRequest : public APIMessage {
 public:
  bool decode_length_delimited(uint32_t field_id, const uint8_t *value, size_t len) override;
  const std::string &get_client_info() const;
  void set_client_info(const std::string &client_info);
  APIMessageType message_type() const override;

 protected:
  std::string client_info_;
};

class ConnectRequest : public APIMessage {
 public:
  bool decode_length_delimited(uint32_t field_id, const uint8_t *value, size_t len) override;
  const std::string &get_password() const;
  void set_password(const std::string &password);
  APIMessageType message_type() const override;

 protected:
  std::string password_;
};

class DeviceInfoRequest : public APIMessage {
 public:
  APIMessageType message_type() const override;
};

class DisconnectRequest : public APIMessage {
 public:
  bool decode_length_delimited(uint32_t field_id, const uint8_t *value, size_t len) override;
  void encode(APIBuffer &buffer) override;
  APIMessageType message_type() const override;
  const std::string &get_reason() const;
  void set_reason(const std::string &reason);

 protected:
  std::string reason_;
};

class DisconnectResponse : public APIMessage {
 public:
  APIMessageType message_type() const override;
};

class PingRequest : public APIMessage {
 public:
  APIMessageType message_type() const override;
};

class PingResponse : public APIMessage {
 public:
  APIMessageType message_type() const override;
};

}  // namespace api

ESPHOME_NAMESPACE_END

#endif  // USE_API

#endif  // ESPHOME_API_BASIC_MESSAGES_H
