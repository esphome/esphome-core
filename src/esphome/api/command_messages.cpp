#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/api/command_messages.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

#ifdef USE_COVER
bool CoverCommandRequest::decode_varint(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 2:
      // bool has_state = 2;
      this->has_state_ = value;
      return true;
    case 3:
      // enum CoverCommand {
      //   OPEN = 0;
      //   CLOSE = 1;
      //   STOP = 2;
      // }
      // CoverCommand command = 3;
      this->command_ = static_cast<cover::CoverCommand>(value);
      return true;
    default:
      return false;
  }
}
bool CoverCommandRequest::decode_32bit(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1:
      // fixed32 key = 1;
      this->key_ = value;
      return true;
    default:
      return false;
  }
}
APIMessageType CoverCommandRequest::message_type() const { return APIMessageType ::COVER_COMMAND_REQUEST; }
uint32_t CoverCommandRequest::get_key() const { return this->key_; }
optional<cover::CoverCommand> CoverCommandRequest::get_command() const {
  if (!this->has_state_)
    return {};
  return this->command_;
}
#endif

#ifdef USE_FAN
bool FanCommandRequest::decode_varint(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 2:
      // bool has_state = 2;
      this->has_state_ = value;
      return true;
    case 3:
      // bool state = 3;
      this->state_ = value;
      return true;
    case 4:
      // bool has_speed = 4;
      this->has_speed_ = value;
      return true;
    case 5:
      // FanSpeed speed = 5;
      this->speed_ = static_cast<fan::FanSpeed>(value);
      return true;
    case 6:
      // bool has_oscillating = 6;
      this->has_oscillating_ = value;
      return true;
    case 7:
      // bool oscillating = 7;
      this->oscillating_ = value;
      return true;
    default:
      return false;
  }
}
bool FanCommandRequest::decode_32bit(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1:
      // fixed32 key = 1;
      this->key_ = value;
      return true;
    default:
      return false;
  }
}
APIMessageType FanCommandRequest::message_type() const { return APIMessageType::FAN_COMMAND_REQUEST; }
uint32_t FanCommandRequest::get_key() const { return this->key_; }
optional<bool> FanCommandRequest::get_state() const {
  if (!this->has_state_)
    return {};
  return this->state_;
}
optional<fan::FanSpeed> FanCommandRequest::get_speed() const {
  if (!this->has_speed_)
    return {};
  return this->speed_;
}
optional<bool> FanCommandRequest::get_oscillating() const {
  if (!this->has_oscillating_)
    return {};
  return this->oscillating_;
}
#endif

#ifdef USE_LIGHT
bool LightCommandRequest::decode_varint(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 2:
      // bool has_state = 2;
      this->has_state_ = value;
      return true;
    case 3:
      // bool state = 3;
      this->state_ = value;
      return true;
    case 4:
      // bool has_brightness = 4;
      this->has_brightness_ = value;
      return true;
    case 6:
      // bool has_rgb = 6;
      this->has_rgb_ = value;
      return true;
    case 10:
      // bool has_white = 10;
      this->has_white_ = value;
      return true;
    case 12:
      // bool has_color_temperature = 12;
      this->has_color_temperature_ = value;
      return true;
    case 14:
      // bool has_transition_length = 14;
      this->has_transition_length_ = value;
      return true;
    case 15:
      // uint32 transition_length = 15;
      this->transition_length_ = value;
      return true;
    case 16:
      // bool has_flash_length = 16;
      this->has_flash_length_ = value;
      return true;
    case 17:
      // uint32 flash_length = 17;
      this->flash_length_ = value;
      return true;
    case 18:
      // bool has_effect = 18;
      this->has_effect_ = value;
      return true;
    default:
      return false;
  }
}
bool LightCommandRequest::decode_length_delimited(uint32_t field_id, const uint8_t *value, size_t len) {
  switch (field_id) {
    case 19:
      // string effect = 19;
      this->effect_ = as_string(value, len);
      return true;
    default:
      return false;
  }
}
bool LightCommandRequest::decode_32bit(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1:
      // fixed32 key = 1;
      this->key_ = value;
      return true;
    case 5:
      // float brightness = 5;
      this->brightness_ = as_float(value);
      return true;
    case 7:
      // float red = 7;
      this->red_ = as_float(value);
      return true;
    case 8:
      // float green = 8;
      this->green_ = as_float(value);
      return true;
    case 9:
      // float blue = 9;
      this->blue_ = as_float(value);
      return true;
    case 11:
      // float white = 11;
      this->white_ = as_float(value);
      return true;
    case 13:
      // float color_temperature = 13;
      this->color_temperature_ = as_float(value);
      return true;
    default:
      return false;
  }
}
APIMessageType LightCommandRequest::message_type() const { return APIMessageType::LIGHT_COMMAND_REQUEST; }
uint32_t LightCommandRequest::get_key() const { return this->key_; }
optional<bool> LightCommandRequest::get_state() const {
  if (!this->has_state_)
    return {};
  return this->state_;
}
optional<float> LightCommandRequest::get_brightness() const {
  if (!this->has_brightness_)
    return {};
  return this->brightness_;
}
optional<float> LightCommandRequest::get_red() const {
  if (!this->has_rgb_)
    return {};
  return this->red_;
}
optional<float> LightCommandRequest::get_green() const {
  if (!this->has_rgb_)
    return {};
  return this->green_;
}
optional<float> LightCommandRequest::get_blue() const {
  if (!this->has_rgb_)
    return {};
  return this->blue_;
}
optional<float> LightCommandRequest::get_white() const {
  if (!this->has_white_)
    return {};
  return this->white_;
}
optional<float> LightCommandRequest::get_color_temperature() const {
  if (!this->has_color_temperature_)
    return {};
  return this->color_temperature_;
}
optional<uint32_t> LightCommandRequest::get_transition_length() const {
  if (!this->has_transition_length_)
    return {};
  return this->transition_length_;
}
optional<uint32_t> LightCommandRequest::get_flash_length() const {
  if (!this->has_flash_length_)
    return {};
  return this->flash_length_;
}
optional<std::string> LightCommandRequest::get_effect() const {
  if (!this->has_effect_)
    return {};
  return this->effect_;
}
#endif

#ifdef USE_SWITCH
bool SwitchCommandRequest::decode_varint(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 2:
      // bool state = 2;
      this->state_ = value;
      return true;
    default:
      return false;
  }
}
bool SwitchCommandRequest::decode_32bit(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1:
      // fixed32 key = 1;
      this->key_ = value;
      return true;
    default:
      return false;
  }
}
APIMessageType SwitchCommandRequest::message_type() const { return APIMessageType::SWITCH_COMMAND_REQUEST; }
uint32_t SwitchCommandRequest::get_key() const { return this->key_; }
bool SwitchCommandRequest::get_state() const { return this->state_; }
#endif

#ifdef USE_ESP32_CAMERA
bool CameraImageRequest::get_single() const { return this->single_; }
bool CameraImageRequest::get_stream() const { return this->stream_; }
bool CameraImageRequest::decode_varint(uint32_t field_id, uint32_t value) {
  switch (field_id) {
    case 1:
      // bool single = 1;
      this->single_ = value;
      return true;
    case 2:
      // bool stream = 2;
      this->stream_ = value;
      return true;
    default:
      return false;
  }
}
APIMessageType CameraImageRequest::message_type() const { return APIMessageType::CAMERA_IMAGE_REQUEST; }
#endif

}  // namespace api

ESPHOME_NAMESPACE_END

#endif  // USE_API
