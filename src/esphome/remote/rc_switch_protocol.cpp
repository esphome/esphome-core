#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/rc_switch_protocol.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

RCSwitchProtocol rc_switch_protocols[8] = {RCSwitchProtocol(0, 0, 0, 0, 0, 0, false),
                                           RCSwitchProtocol(350, 10850, 350, 1050, 1050, 350, false),
                                           RCSwitchProtocol(650, 6500, 650, 1300, 1300, 650, false),
                                           RCSwitchProtocol(3000, 7100, 400, 1100, 900, 600, false),
                                           RCSwitchProtocol(380, 2280, 380, 1140, 1140, 380, false),
                                           RCSwitchProtocol(3000, 7000, 500, 1000, 1000, 500, false),
                                           RCSwitchProtocol(10350, 450, 450, 900, 900, 450, true),
                                           RCSwitchProtocol(300, 9300, 150, 900, 900, 150, false)};

RCSwitchProtocol::RCSwitchProtocol(uint32_t sync_high, uint32_t sync_low, uint32_t zero_high, uint32_t zero_low,
                                   uint32_t one_high, uint32_t one_low, bool inverted)
    : sync_high_(sync_high),
      sync_low_(sync_low),
      zero_high_(zero_high),
      zero_low_(zero_low),
      one_high_(one_high),
      one_low_(one_low),
      inverted_(inverted) {}

#ifdef USE_REMOTE_TRANSMITTER
void RCSwitchProtocol::one(RemoteTransmitData *data) const {
  if (!this->inverted_) {
    data->mark(this->one_high_);
    data->space(this->one_low_);
  } else {
    data->space(this->one_high_);
    data->mark(this->one_low_);
  }
}
void RCSwitchProtocol::zero(RemoteTransmitData *data) const {
  if (!this->inverted_) {
    data->mark(this->zero_high_);
    data->space(this->zero_low_);
  } else {
    data->space(this->zero_high_);
    data->mark(this->zero_low_);
  }
}
void RCSwitchProtocol::sync(RemoteTransmitData *data) const {
  if (!this->inverted_) {
    data->mark(this->sync_high_);
    data->space(this->sync_low_);
  } else {
    data->space(this->sync_high_);
    data->mark(this->sync_low_);
  }
}
void RCSwitchProtocol::transmit(RemoteTransmitData *data, uint32_t code, uint8_t len) const {
  for (int16_t i = len - 1; i >= 0; i--) {
    if (code & (1 << i))
      this->one(data);
    else
      this->zero(data);
  }
  this->sync(data);
}
#endif

#ifdef USE_REMOTE_RECEIVER
bool RCSwitchProtocol::expect_one(RemoteReceiveData *data) const {
  if (!this->inverted_) {
    if (!data->peek_mark(this->one_high_))
      return false;
    if (!data->peek_space(this->one_low_, 1))
      return false;
  } else {
    if (!data->peek_space(this->one_high_))
      return false;
    if (!data->peek_mark(this->one_low_, 1))
      return false;
  }
  data->advance(2);
  return true;
}
bool RCSwitchProtocol::expect_zero(RemoteReceiveData *data) const {
  if (!this->inverted_) {
    if (!data->peek_mark(this->zero_high_))
      return false;
    if (!data->peek_space(this->zero_low_, 1))
      return false;
  } else {
    if (!data->peek_space(this->zero_high_))
      return false;
    if (!data->peek_mark(this->zero_low_, 1))
      return false;
  }
  data->advance(2);
  return true;
}
bool RCSwitchProtocol::expect_sync(RemoteReceiveData *data) const {
  if (!this->inverted_) {
    if (!data->peek_mark(this->sync_high_))
      return false;
    if (!data->peek_space(this->sync_low_, 1))
      return false;
  } else {
    if (!data->peek_space(this->sync_high_))
      return false;
    if (!data->peek_mark(this->sync_low_, 1))
      return false;
  }
  data->advance(2);
  return true;
}
bool RCSwitchProtocol::decode(RemoteReceiveData *data, uint32_t *out_data, uint8_t *out_nbits) const {
  // ignore if sync doesn't exist
  this->expect_sync(data);

  *out_data = 0;
  for (*out_nbits = 1; *out_nbits < 32; *out_nbits += 1) {
    if (this->expect_zero(data)) {
      *out_data <<= 1;
      *out_data |= 0;
    } else if (this->expect_one(data)) {
      *out_data <<= 1;
      *out_data |= 1;
    } else {
      *out_nbits -= 1;
      return *out_nbits >= 8;
    }
  }
  return true;
}
#endif

void RCSwitchProtocol::simple_code_to_tristate(uint16_t code, uint8_t nbits, uint32_t *out_code) {
  *out_code = 0;
  for (int8_t i = nbits - 1; i >= 0; i--) {
    *out_code <<= 2;
    if (code & (1 << i))
      *out_code |= 0b01;
    else
      *out_code |= 0b00;
  }
}
void RCSwitchProtocol::type_a_code(uint8_t switch_group, uint8_t switch_device, bool state, uint32_t *out_code,
                                   uint8_t *out_nbits) {
  uint16_t code = 0;
  code |= (switch_group & 0b0001) ? 0 : 0b1000;
  code |= (switch_group & 0b0010) ? 0 : 0b0100;
  code |= (switch_group & 0b0100) ? 0 : 0b0010;
  code |= (switch_group & 0b1000) ? 0 : 0b0001;
  code <<= 4;
  code |= (switch_device & 0b0001) ? 0 : 0b1000;
  code |= (switch_device & 0b0010) ? 0 : 0b0100;
  code |= (switch_device & 0b0100) ? 0 : 0b0010;
  code |= (switch_device & 0b1000) ? 0 : 0b0001;
  code <<= 2;
  code |= state ? 0b01 : 0b10;
  simple_code_to_tristate(code, 10, out_code);
  *out_nbits = 20;
}
void RCSwitchProtocol::type_b_code(uint8_t address_code, uint8_t channel_code, bool state, uint32_t *out_code,
                                   uint8_t *out_nbits) {
  uint16_t code = 0;
  code |= (address_code == 1) ? 0 : 0b1000;
  code |= (address_code == 2) ? 0 : 0b0100;
  code |= (address_code == 3) ? 0 : 0b0010;
  code |= (address_code == 4) ? 0 : 0b0001;
  code <<= 4;
  code |= (channel_code == 1) ? 0 : 0b1000;
  code |= (channel_code == 2) ? 0 : 0b0100;
  code |= (channel_code == 3) ? 0 : 0b0010;
  code |= (channel_code == 4) ? 0 : 0b0001;
  code <<= 4;
  code |= 0b1110;
  code |= state ? 0b1 : 0b0;
  simple_code_to_tristate(code, 12, out_code);
  *out_nbits = 24;
}
void RCSwitchProtocol::type_c_code(uint8_t family, uint8_t group, uint8_t device, bool state, uint32_t *out_code,
                                   uint8_t *out_nbits) {
  uint16_t code = 0;
  code |= (family & 0b0001) ? 0b1000 : 0;
  code |= (family & 0b0010) ? 0b0100 : 0;
  code |= (family & 0b0100) ? 0b0010 : 0;
  code |= (family & 0b1000) ? 0b0001 : 0;
  code <<= 4;
  code |= ((device - 1) & 0b01) ? 0b1000 : 0;
  code |= ((device - 1) & 0b10) ? 0b0100 : 0;
  code |= ((group - 1) & 0b01) ? 0b0010 : 0;
  code |= ((group - 1) & 0b10) ? 0b0001 : 0;
  code <<= 4;
  code |= 0b0110;
  code |= state ? 0b1 : 0b0;
  simple_code_to_tristate(code, 12, out_code);
  *out_nbits = 24;
}
void RCSwitchProtocol::type_d_code(uint8_t group, uint8_t device, bool state, uint32_t *out_code, uint8_t *out_nbits) {
  *out_code = 0;
  *out_code |= (group == 0) ? 0b11000000 : 0b01000000;
  *out_code |= (group == 1) ? 0b00110000 : 0b00010000;
  *out_code |= (group == 2) ? 0b00001100 : 0b00000100;
  *out_code |= (group == 3) ? 0b00000011 : 0b00000001;
  *out_code <<= 6;
  *out_code |= (device == 1) ? 0b110000 : 0b010000;
  *out_code |= (device == 2) ? 0b001100 : 0b000100;
  *out_code |= (device == 3) ? 0b000011 : 0b000001;
  *out_code <<= 6;
  *out_code |= 0b000000;
  *out_code <<= 4;
  *out_code |= state ? 0b1100 : 0b0011;
  *out_nbits = 24;
}

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
