#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/api/util.h"
#include "esphome/api/api_server.h"
#include "esphome/api/user_services.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

APIBuffer::APIBuffer(std::vector<uint8_t> *buffer)
  : buffer_(buffer) {

}
size_t APIBuffer::get_length() const {
  return this->buffer_->size();
}
void APIBuffer::write(uint8_t value) {
  this->buffer_->push_back(value);
}
void APIBuffer::encode_uint32(uint32_t field, uint32_t value) {
  if (value == 0)
    return;

  this->encode_field_(field, 0);
  this->encode_varint_(value);
}
void APIBuffer::encode_int32(uint32_t field, int32_t value) {
  this->encode_uint32(field, static_cast<uint32_t>(value));
}
void APIBuffer::encode_bool(uint32_t field, bool value) {
  if (!value)
    return;

  this->encode_field_(field, 0);
  this->write(0x01);
}
void APIBuffer::encode_string(uint32_t field, const std::string &value) {
  this->encode_string(field, value.data(), value.size());
}
void APIBuffer::encode_string(uint32_t field, const char *string, size_t len) {
  if (len == 0)
    return;

  this->encode_field_(field, 2);
  this->encode_varint_(len);
  const uint8_t *data = reinterpret_cast<const uint8_t *>(string);
  for (size_t i = 0; i < len; i++) {
    this->write(data[i]);
  }
}
void APIBuffer::encode_fixed32(uint32_t field, uint32_t value) {
  if (value == 0)
    return;

  this->encode_field_(field, 5);
  this->write((value >> 0) & 0xFF);
  this->write((value >> 8) & 0xFF);
  this->write((value >> 16) & 0xFF);
  this->write((value >> 24) & 0xFF);
}
void APIBuffer::encode_float(uint32_t field, float value) {
  if (value == 0.0f)
    return;

  union {
    float value_f;
    uint32_t value_raw;
  } val;
  val.value_f = value;
  this->encode_fixed32(field, val.value_raw);
}
void APIBuffer::encode_field_(uint32_t field, uint32_t type) {
  uint32_t val = (field << 3) | (type & 0b111);
  this->encode_varint_(val);
}
void APIBuffer::encode_varint_(uint32_t value) {
  if (value <= 0x7F) {
    this->write(value);
    return;
  }

  while (value) {
    uint8_t temp = value & 0x7F;
    value >>= 7;
    if (value) {
      this->write(temp | 0x80);
    } else {
      this->write(temp);
    }
  }
}
void APIBuffer::encode_sint32(uint32_t field, int32_t value) {
  if (value < 0)
    this->encode_uint32(field, ~(uint32_t(value) << 1));
  else
    this->encode_uint32(field, uint32_t(value) << 1);
}
void APIBuffer::encode_nameable(Nameable *nameable) {
  // string object_id = 1;
  this->encode_string(1, nameable->get_object_id());
  // fixed32 key = 2;
  this->encode_fixed32(2, nameable->get_object_id_hash());
  // string name = 3;
  this->encode_string(3, nameable->get_name());
}
uint32_t APIBuffer::varint_length_(uint32_t value) {
  if (value <= 0x7F) {
    return 1;
  }

  uint32_t bytes = 0;
  while (value) {
    value >>= 7;
    bytes++;
  }

  return bytes;
}
size_t APIBuffer::begin_nested(uint32_t field) {
  this->encode_field_(field, 2);
  return this->buffer_->size();
}
void APIBuffer::end_nested(size_t begin_index) {
  const uint32_t nested_length = this->buffer_->size() - begin_index;
  // add varint
  std::vector<uint8_t> var;
  uint32_t val = nested_length;
  if (val <= 0x7F) {
    var.push_back(val);
  } else {
    while (val) {
      uint8_t temp = val & 0x7F;
      val >>= 7;
      if (val) {
        var.push_back(temp | 0x80);
      } else {
        var.push_back(temp);
      }
    }
  }
  this->buffer_->insert(this->buffer_->begin() + begin_index, var.begin(), var.end());
}

optional<uint32_t> proto_decode_varuint32(const uint8_t *buf, size_t len, uint32_t *consumed) {
  if (len == 0)
    return {};

  uint32_t result = 0;
  uint8_t bitpos = 0;

  for (uint32_t i = 0; i < len; i++) {
    uint8_t val = buf[i];
    result |= uint32_t(val & 0x7F) << bitpos;
    bitpos += 7;
    if ((val & 0x80) == 0) {
      if (consumed != nullptr) {
        *consumed = i + 1;
      }
      return result;
    }
  }

  return {};
}

std::string as_string(const uint8_t *value, size_t len) {
  return std::string(reinterpret_cast<const char *>(value), len);
}

int32_t as_sint32(uint32_t val) {
  if (val & 1)
    return uint32_t(~(val >> 1));
  else
    return uint32_t(val >> 1);
}

float as_float(uint32_t val) {
  union {
    uint32_t raw;
    float value;
  } x;
  x.raw = val;
  return x.value;
}

ComponentIterator::ComponentIterator(APIServer *server) : server_(server) {

}
void ComponentIterator::begin() {
  this->state_ = IteratorState::BEGIN;
  this->at_ = 0;
}
void ComponentIterator::advance() {
  bool advance_platform = false;
  bool success = true;
  switch (this->state_) {
    case IteratorState::NONE:
      // not started
      return;
    case IteratorState::BEGIN:
      if (this->on_begin()) {
        advance_platform = true;
      } else {
        return;
      }
      break;
#ifdef USE_BINARY_SENSOR
    case IteratorState::BINARY_SENSOR:
      if (this->at_ >= this->server_->binary_sensors_.size()) {
        advance_platform = true;
      } else {
        auto *binary_sensor = this->server_->binary_sensors_[this->at_];
        if (binary_sensor->is_internal()) {
          success = true;
          break;
        } else {
          success = this->on_binary_sensor(binary_sensor);
        }
      }
      break;
#endif
#ifdef USE_COVER
    case IteratorState::COVER:
      if (this->at_ >= this->server_->covers_.size()) {
        advance_platform = true;
      } else {
        auto *cover = this->server_->covers_[this->at_];
        if (cover->is_internal()) {
          success = true;
          break;
        } else {
          success = this->on_cover(cover);
        }
      }
      break;
#endif
#ifdef USE_FAN
    case IteratorState::FAN:
      if (this->at_ >= this->server_->fans_.size()) {
        advance_platform = true;
      } else {
        auto *fan = this->server_->fans_[this->at_];
        if (fan->is_internal()) {
          success = true;
          break;
        } else {
          success = this->on_fan(fan);
        }
      }
      break;
#endif
#ifdef USE_LIGHT
    case IteratorState::LIGHT:
      if (this->at_ >= this->server_->lights_.size()) {
        advance_platform = true;
      } else {
        auto *light = this->server_->lights_[this->at_];
        if (light->is_internal()) {
          success = true;
          break;
        } else {
          success = this->on_light(light);
        }
      }
      break;
#endif
#ifdef USE_SENSOR
    case IteratorState::SENSOR:
      if (this->at_ >= this->server_->sensors_.size()) {
        advance_platform = true;
      } else {
        auto *sensor = this->server_->sensors_[this->at_];
        if (sensor->is_internal()) {
          success = true;
          break;
        } else {
          success = this->on_sensor(sensor);
        }
      }
      break;
#endif
#ifdef USE_SWITCH
    case IteratorState::SWITCH:
      if (this->at_ >= this->server_->switches_.size()) {
        advance_platform = true;
      } else {
        auto *switch_ = this->server_->switches_[this->at_];
        if (switch_->is_internal()) {
          success = true;
          break;
        } else {
          success = this->on_switch(switch_);
        }
      }
      break;
#endif
#ifdef USE_TEXT_SENSOR
    case IteratorState::TEXT_SENSOR:
      if (this->at_ >= this->server_->text_sensors_.size()) {
        advance_platform = true;
      } else {
        auto *text_sensor = this->server_->text_sensors_[this->at_];
        if (text_sensor->is_internal()) {
          success = true;
          break;
        } else {
          success = this->on_text_sensor(text_sensor);
        }
      }
      break;
#endif
    case IteratorState ::SERVICE:
      if (this->at_ >= this->server_->get_user_services().size()) {
        advance_platform = true;
      } else {
        auto *service = this->server_->get_user_services()[this->at_];
        success = this->on_service(service);
      }
      break;
    case IteratorState::MAX:
      if (this->on_end()) {
        this->state_ = IteratorState::NONE;
      }
      return;
  }

  if (advance_platform) {
    this->state_ = static_cast<IteratorState>(static_cast<uint32_t>(this->state_) + 1);
    this->at_ = 0;
  } else if (success) {
    this->at_++;
  }
}
bool ComponentIterator::on_end() {
  return true;
}
bool ComponentIterator::on_begin() {
  return true;
}
bool ComponentIterator::on_service(UserServiceDescriptor *service) {

}

} // namespace api

ESPHOME_NAMESPACE_END

#endif //USE_API
