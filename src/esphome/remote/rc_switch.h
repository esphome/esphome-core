#ifndef ESPHOME_REMOTE_RC_SWITCH_H
#define ESPHOME_REMOTE_RC_SWITCH_H

#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/remote_receiver.h"
#include "esphome/remote/remote_transmitter.h"
#include "esphome/remote/rc_switch_protocol.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class RCSwitchRawTransmitter : public RemoteTransmitter {
 public:
  RCSwitchRawTransmitter(const std::string &name, RCSwitchProtocol a_protocol, uint32_t code, uint8_t nbits);

  void to_data(RemoteTransmitData *data) override;

 protected:
  RCSwitchProtocol protocol_;
  uint32_t code_;
  uint8_t nbits_;
};

void encode_rc_switch_raw(RemoteTransmitData *data, uint32_t code, uint8_t nbits,
                          RCSwitchProtocol protocol = rc_switch_protocols[1]);

void encode_rc_switch_raw(RemoteTransmitData *data, const char *code,
                          RCSwitchProtocol protocol = rc_switch_protocols[1]);

void encode_rc_switch_raw_tristate(RemoteTransmitData *data, const char *code,
                                   RCSwitchProtocol protocol = rc_switch_protocols[1]);

class RCSwitchTypeATransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeATransmitter(const std::string &name, RCSwitchProtocol a_protocol, uint8_t switch_group,
                           uint8_t switch_device, bool state);
};

void encode_rc_switch_type_a(RemoteTransmitData *data, uint8_t switch_group, uint8_t switch_device, bool state,
                             RCSwitchProtocol protocol = rc_switch_protocols[1]);

void encode_rc_switch_type_a(RemoteTransmitData *data, const char *switch_group, const char *switch_device, bool state,
                             RCSwitchProtocol protocol = rc_switch_protocols[1]);

class RCSwitchTypeBTransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeBTransmitter(const std::string &name, RCSwitchProtocol a_protocol, uint8_t address_code,
                           uint8_t channel_code, bool state);
};

void encode_rc_switch_type_b(RemoteTransmitData *data, uint8_t address, uint8_t channel, bool state,
                             RCSwitchProtocol protocol = rc_switch_protocols[1]);

class RCSwitchTypeCTransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeCTransmitter(const std::string &name, RCSwitchProtocol a_protocol, uint8_t family, uint8_t group,
                           uint8_t device, bool state);
};

void encode_rc_switch_type_c(RemoteTransmitData *data, uint8_t family, uint8_t group, uint8_t device, bool state,
                             RCSwitchProtocol protocol = rc_switch_protocols[1]);

void encode_rc_switch_type_c(RemoteTransmitData *data, char family, uint8_t group, uint8_t device, bool state,
                             RCSwitchProtocol protocol = rc_switch_protocols[1]);

class RCSwitchTypeDTransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeDTransmitter(const std::string &name, RCSwitchProtocol a_protocol, uint8_t group, uint8_t device,
                           bool state);
};

void encode_rc_switch_type_d(RemoteTransmitData *data, uint8_t group, uint8_t device, bool state,
                             RCSwitchProtocol protocol = rc_switch_protocols[1]);

void encode_rc_switch_type_d(RemoteTransmitData *data, char group, uint8_t device, bool state,
                             RCSwitchProtocol protocol = rc_switch_protocols[1]);

#endif

#ifdef USE_REMOTE_RECEIVER
class RCSwitchRawReceiver : public RemoteReceiver {
 public:
  RCSwitchRawReceiver(const std::string &name, RCSwitchProtocol a_protocol, uint32_t code, uint8_t nbits);

 protected:
  bool matches(RemoteReceiveData *data) override;

  RCSwitchProtocol protocol_;
  uint32_t code_;
  uint8_t nbits_;
};

class RCSwitchTypeAReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeAReceiver(const std::string &name, RCSwitchProtocol a_protocol, uint8_t switch_group,
                        uint8_t switch_device, bool state);
};

class RCSwitchTypeBReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeBReceiver(const std::string &name, RCSwitchProtocol a_protocol, uint8_t address_code,
                        uint8_t channel_code, bool state);
};

class RCSwitchTypeCReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeCReceiver(const std::string &name, RCSwitchProtocol a_protocol, uint8_t family, uint8_t group,
                        uint8_t device, bool state);
};

class RCSwitchTypeDReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeDReceiver(const std::string &name, RCSwitchProtocol a_protocol, uint8_t group, uint8_t device,
                        bool state);
};

class RCSwitchDumper : public RemoteReceiveDumper {
 public:
  bool dump(RemoteReceiveData *data) override;
};
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_

#endif  // ESPHOME_REMOTE_RC_SWITCH_H
