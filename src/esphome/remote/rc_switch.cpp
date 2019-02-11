#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/rc_switch.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.rc_switch";
#endif

#ifdef USE_REMOTE_TRANSMITTER
RCSwitchRawTransmitter::RCSwitchRawTransmitter(const std::string &name,
                                               RCSwitchProtocol aProtocol,
                                               uint32_t code,
                                               uint8_t nbits)
    : RemoteTransmitter(name), protocol_(aProtocol), code_(code), nbits_(nbits) {}
void RCSwitchRawTransmitter::to_data(RemoteTransmitData *data) {
  this->protocol_.transmit(data, this->code_, this->nbits_);
}

void encode_rc_switch_raw(RemoteTransmitData *data, uint32_t code, uint8_t nbits, RCSwitchProtocol protocol) {
  protocol.transmit(data, code, nbits);
}
void encode_rc_switch_raw(RemoteTransmitData *data, const char *code, RCSwitchProtocol protocol) {
  uint32_t code_ = 0;
  uint8_t nbits = 0;
  for (; code[nbits] != '\0'; nbits++) {
    code_ <<= 1UL;
    code_ |= (code[nbits] != '0');
  }

  protocol.transmit(data, code_, nbits);
}
void encode_rc_switch_raw_tristate(RemoteTransmitData *data, const char *code, RCSwitchProtocol protocol) {
  uint32_t code_ = 0;
  uint8_t nbits = 0;
  for (; code[nbits] != '\0'; nbits++) {
    code_ <<= 2UL;
    switch (code[nbits]) {
      case 'F':
        code_ |= 0b01UL;
        break;
      case '1':
        code_ |= 0b11UL;
        break;
      case '0':
      default:
        code_ |= 0b00UL;
        break;
    }
  }

  protocol.transmit(data, code_, nbits += 2);
}
RCSwitchTypeATransmitter::RCSwitchTypeATransmitter(const std::string &name,
                                                   RCSwitchProtocol aProtocol,
                                                   uint8_t switch_group,
                                                   uint8_t switch_device,
                                                   bool state)
    : RCSwitchRawTransmitter(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_a_code(switch_group, switch_device, state, &this->code_, &this->nbits_);
}

void encode_rc_switch_type_a(RemoteTransmitData *data,
                             uint8_t switch_group,
                             uint8_t switch_device,
                             bool state,
                             RCSwitchProtocol protocol) {
  uint32_t code;
  uint8_t nbits;
  RCSwitchProtocol::type_a_code(switch_group, switch_device, state, &code, &nbits);
  protocol.transmit(data, code, nbits);
}

uint32_t decode_string(const char *str, uint8_t len) {
  uint32_t ret = 0;
  for (uint8_t i = 0; i < len; i++) {
    ret <<= 1UL;
    ret |= str[i] != '0';
  }
  return ret;
}

void encode_rc_switch_type_a(RemoteTransmitData *data,
                             const char *switch_group,
                             const char *switch_device,
                             bool state,
                             RCSwitchProtocol protocol) {
  uint8_t switch_group_ = decode_string(switch_group, 5);
  uint8_t switch_device_ = decode_string(switch_device, 5);

  encode_rc_switch_type_a(data, switch_group_, switch_device_, state, protocol);
}

RCSwitchTypeBTransmitter::RCSwitchTypeBTransmitter(const std::string &name,
                                                   RCSwitchProtocol aProtocol,
                                                   uint8_t address_code,
                                                   uint8_t channel_code,
                                                   bool state)
    : RCSwitchRawTransmitter(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_b_code(address_code, channel_code, state, &this->code_, &this->nbits_);
}
void encode_rc_switch_type_b(RemoteTransmitData *data,
                             uint8_t address,
                             uint8_t channel,
                             bool state,
                             RCSwitchProtocol protocol) {
  uint32_t code;
  uint8_t nbits;
  RCSwitchProtocol::type_b_code(address, channel, state, &code, &nbits);
  protocol.transmit(data, code, nbits);
}
RCSwitchTypeCTransmitter::RCSwitchTypeCTransmitter(const std::string &name,
                                                   RCSwitchProtocol aProtocol,
                                                   uint8_t family,
                                                   uint8_t group,
                                                   uint8_t device,
                                                   bool state)
    : RCSwitchRawTransmitter(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_c_code(family, group, device, state, &this->code_, &this->nbits_);
}
void encode_rc_switch_type_c(RemoteTransmitData *data,
                             uint8_t family,
                             uint8_t group,
                             uint8_t device,
                             bool state,
                             RCSwitchProtocol protocol) {
  uint32_t code;
  uint8_t nbits;
  RCSwitchProtocol::type_c_code(family, group, device, state, &code, &nbits);
  protocol.transmit(data, code, nbits);
}
void encode_rc_switch_type_c(RemoteTransmitData *data,
                                     char family,
                                     uint8_t group,
                                     uint8_t device,
                                     bool state,
                                     RCSwitchProtocol protocol) {
  uint8_t family_ = static_cast<uint8_t>(family - 'a');
  encode_rc_switch_type_c(data, family_, group, device, state, protocol);
}
RCSwitchTypeDTransmitter::RCSwitchTypeDTransmitter(const std::string &name,
                                                   RCSwitchProtocol aProtocol,
                                                   uint8_t group,
                                                   uint8_t device,
                                                   bool state)
    : RCSwitchRawTransmitter(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_d_code(group, device, state, &this->code_, &this->nbits_);
}
void encode_rc_switch_type_d(RemoteTransmitData *data,
                             uint8_t group,
                             uint8_t device,
                             bool state,
                             RCSwitchProtocol protocol) {
  uint32_t code;
  uint8_t nbits;
  RCSwitchProtocol::type_d_code(group, device, state, &code, &nbits);
  protocol.transmit(data, code, nbits);
}
void encode_rc_switch_type_d(RemoteTransmitData *data,
                             char group,
                             uint8_t device,
                             bool state,
                             RCSwitchProtocol protocol) {
  uint8_t group_ = static_cast<uint8_t>(group - 'a');
  encode_rc_switch_type_d(data, group_, device, state, protocol);
}
#endif

#ifdef USE_REMOTE_RECEIVER
RCSwitchRawReceiver::RCSwitchRawReceiver(const std::string &name,
                                         RCSwitchProtocol aProtocol,
                                         uint32_t code,
                                         uint8_t nbits)
    : RemoteReceiver(name), protocol_(aProtocol), code_(code), nbits_(nbits) {}
bool RCSwitchRawReceiver::matches(RemoteReceiveData *data) {
  uint32_t decoded_code;
  uint8_t decoded_nbits;
  if (!this->protocol_.decode(data, &decoded_code, &decoded_nbits))
    return false;

  return decoded_nbits == this->nbits_ && decoded_code == this->code_;
}
RCSwitchTypeAReceiver::RCSwitchTypeAReceiver(const std::string &name,
                                             RCSwitchProtocol aProtocol,
                                             uint8_t switch_group,
                                             uint8_t switch_device,
                                             bool state)
    : RCSwitchRawReceiver(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_a_code(switch_group, switch_device, state, &this->code_, &this->nbits_);
}
RCSwitchTypeBReceiver::RCSwitchTypeBReceiver(const std::string &name,
                                             RCSwitchProtocol aProtocol,
                                             uint8_t address_code,
                                             uint8_t channel_code,
                                             bool state)
    : RCSwitchRawReceiver(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_b_code(address_code, channel_code, state, &this->code_, &this->nbits_);
}
RCSwitchTypeCReceiver::RCSwitchTypeCReceiver(const std::string &name,
                                             RCSwitchProtocol aProtocol,
                                             uint8_t family,
                                             uint8_t group,
                                             uint8_t device,
                                             bool state)
    : RCSwitchRawReceiver(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_c_code(family, group, device, state, &this->code_, &this->nbits_);
}
RCSwitchTypeDReceiver::RCSwitchTypeDReceiver(const std::string &name,
                                             RCSwitchProtocol aProtocol,
                                             uint8_t group,
                                             uint8_t device,
                                             bool state)
    : RCSwitchRawReceiver(name, aProtocol, 0, 0) {
  RCSwitchProtocol::type_d_code(group, device, state, &this->code_, &this->nbits_);
}

bool RCSwitchDumper::dump(RemoteReceiveData *data) {
  bool ret = false;
  for (uint8_t i = 1; i <= 7; i++) {
    data->reset_index();
    uint32_t out_data;
    uint8_t out_nbits;
    RCSwitchProtocol *protocol = &rc_switch_protocols[i];
    if (protocol->decode(data, &out_data, &out_nbits)) {
      char buffer[32];
      for (uint8_t j = 0; j < out_nbits; j++)
        buffer[j] = (out_data & (1 << (out_nbits - j - 1))) ? '1' : '0';

      buffer[out_nbits] = '\0';
      ESP_LOGD(TAG, "Received RCSwitch: protocol=%u data='%s'", i, buffer);
      ret = true;
    }
  }

  return ret;
}
#endif

} // namespace remote

ESPHOME_NAMESPACE_END

#endif //USE_REMOTE
