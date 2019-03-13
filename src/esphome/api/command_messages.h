#ifndef ESPHOME_API_COMMAND_MESSAGES_H
#define ESPHOME_API_COMMAND_MESSAGES_H

#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/component.h"
#include "esphome/api/api_message.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

#ifdef USE_COVER
class CoverCommandRequest : public APIMessage {
 public:
  bool decode_varint(uint32_t field_id, uint32_t value) override;
  bool decode_32bit(uint32_t field_id, uint32_t value) override;
  APIMessageType message_type() const override;
  uint32_t get_key() const;
  optional<cover::CoverCommand> get_command() const;

 protected:
  uint32_t key_{0};
  bool has_state_{false};
  cover::CoverCommand command_{cover::COVER_COMMAND_OPEN};
};
#endif

#ifdef USE_FAN
class FanCommandRequest : public APIMessage {
 public:
  bool decode_varint(uint32_t field_id, uint32_t value) override;
  bool decode_32bit(uint32_t field_id, uint32_t value) override;
  APIMessageType message_type() const override;
  uint32_t get_key() const;
  optional<bool> get_state() const;
  optional<fan::FanSpeed> get_speed() const;
  optional<bool> get_oscillating() const;

 protected:
  uint32_t key_{0};
  bool has_state_{false};
  bool state_{false};
  bool has_speed_{false};
  fan::FanSpeed speed_{fan::FAN_SPEED_LOW};
  bool has_oscillating_{false};
  bool oscillating_{false};
};
#endif

#ifdef USE_LIGHT
class LightCommandRequest : public APIMessage {
 public:
  bool decode_varint(uint32_t field_id, uint32_t value) override;
  bool decode_length_delimited(uint32_t field_id, const uint8_t *value, size_t len) override;
  bool decode_32bit(uint32_t field_id, uint32_t value) override;
  APIMessageType message_type() const override;
  uint32_t get_key() const;
  optional<bool> get_state() const;
  optional<float> get_brightness() const;
  optional<float> get_red() const;
  optional<float> get_green() const;
  optional<float> get_blue() const;
  optional<float> get_white() const;
  optional<float> get_color_temperature() const;
  optional<uint32_t> get_transition_length() const;
  optional<uint32_t> get_flash_length() const;
  optional<std::string> get_effect() const;

 protected:
  uint32_t key_{0};
  bool has_state_{false};
  bool state_{false};
  bool has_brightness_{false};
  float brightness_{0.0f};
  bool has_rgb_{false};
  float red_{0.0f};
  float green_{0.0f};
  float blue_{0.0f};
  bool has_white_{false};
  float white_{0.0f};
  bool has_color_temperature_{false};
  float color_temperature_{0.0f};
  bool has_transition_length_{false};
  uint32_t transition_length_{0};
  bool has_flash_length_{false};
  uint32_t flash_length_{0};
  bool has_effect_{false};
  std::string effect_{};
};
#endif

#ifdef USE_SWITCH
class SwitchCommandRequest : public APIMessage {
 public:
  bool decode_varint(uint32_t field_id, uint32_t value) override;
  bool decode_32bit(uint32_t field_id, uint32_t value) override;
  APIMessageType message_type() const override;
  uint32_t get_key() const;
  bool get_state() const;

 protected:
  uint32_t key_{0};
  bool state_{false};
};
#endif

#ifdef USE_ESP32_CAMERA
class CameraImageRequest : public APIMessage {
 public:
  bool decode_varint(uint32_t field_id, uint32_t value) override;
  bool get_single() const;
  bool get_stream() const;
  APIMessageType message_type() const override;

 protected:
  bool single_{false};
  bool stream_{false};
};
#endif

}  // namespace api

ESPHOME_NAMESPACE_END

#endif  // USE_API

#endif  // ESPHOME_API_COMMAND_MESSAGES_H
