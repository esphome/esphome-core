#ifndef ESPHOMELIB_REMOTE_RC_SWITCH_H
#define ESPHOMELIB_REMOTE_RC_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_protocol.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

class RCSwitchProtocol {
 public:
  RCSwitchProtocol(uint32_t sync_high,
                   uint32_t sync_low,
                   uint32_t zero_high,
                   uint32_t zero_low,
                   uint32_t one_high,
                   uint32_t one_low,
                   bool inverted);

#ifdef USE_REMOTE_TRANSMITTER
  void one(RemoteTransmitData *data) const;

  void zero(RemoteTransmitData *data) const;

  void sync(RemoteTransmitData *data) const;

  void transmit(RemoteTransmitData *data, uint32_t code, uint8_t len) const;
#endif

#ifdef USE_REMOTE_RECEIVER
  bool expect_one(RemoteReceiveData *data) const;

  bool expect_zero(RemoteReceiveData *data) const;

  bool expect_sync(RemoteReceiveData *data) const;

  bool decode(RemoteReceiveData *data, uint32_t *out_data, uint8_t *out_nbits) const;
#endif

  static void simple_code_to_tristate(uint16_t code, uint8_t nbits, uint32_t *out_code);

  static void type_a_code(uint8_t switch_group, uint8_t switch_device, bool state, uint32_t *out_code, uint8_t *out_nbits);

  static void type_b_code(uint8_t address_code, uint8_t channel_code, bool state, uint32_t *out_code, uint8_t *out_nbits);

  static void type_c_code(uint8_t family, uint8_t group, uint8_t device, bool state, uint32_t *out_code, uint8_t *out_nbits);

  static void type_d_code(uint8_t group, uint8_t device, bool state, uint32_t *out_code, uint8_t *out_nbits);

 protected:
  uint32_t sync_high_;
  uint32_t sync_low_;
  uint32_t zero_high_;
  uint32_t zero_low_;
  uint32_t one_high_;
  uint32_t one_low_;
  bool inverted_;
};

extern RCSwitchProtocol rc_switch_protocols[8];

#ifdef USE_REMOTE_TRANSMITTER
class RCSwitchRawTransmitter : public RemoteTransmitter {
 public:
  RCSwitchRawTransmitter(const std::string &name, RCSwitchProtocol aProtocol, uint32_t code, uint8_t nbits);

  void to_data(RemoteTransmitData *data) override;
 protected:
  RCSwitchProtocol protocol_;
  uint32_t code_;
  uint8_t nbits_;
};

class RCSwitchTypeATransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeATransmitter(const std::string &name, RCSwitchProtocol aProtocol, uint8_t switch_group, uint8_t switch_device, bool state);
};

class RCSwitchTypeBTransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeBTransmitter(const std::string &name, RCSwitchProtocol aProtocol, uint8_t address_code, uint8_t channel_code, bool state);
};

class RCSwitchTypeCTransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeCTransmitter(const std::string &name, RCSwitchProtocol aProtocol, uint8_t family, uint8_t group, uint8_t device, bool state);
};

class RCSwitchTypeDTransmitter : public RCSwitchRawTransmitter {
 public:
  RCSwitchTypeDTransmitter(const std::string &name, RCSwitchProtocol aProtocol, uint8_t group, uint8_t device, bool state);
};

#endif

#ifdef USE_REMOTE_RECEIVER
class RCSwitchRawReceiver : public RemoteReceiver {
 public:
  RCSwitchRawReceiver(const std::string &name, RCSwitchProtocol aProtocol, uint32_t code, uint8_t nbits);

  bool matches(RemoteReceiveData *data) override;

 protected:
  RCSwitchProtocol protocol_;
  uint32_t code_;
  uint8_t nbits_;
};

class RCSwitchTypeAReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeAReceiver(const std::string &name, RCSwitchProtocol aProtocol, uint8_t switch_group, uint8_t switch_device, bool state);
};

class RCSwitchTypeBReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeBReceiver(const std::string &name, RCSwitchProtocol aProtocol, uint8_t address_code, uint8_t channel_code, bool state);
};

class RCSwitchTypeCReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeCReceiver(const std::string &name, RCSwitchProtocol aProtocol, uint8_t family, uint8_t group, uint8_t device, bool state);
};

class RCSwitchTypeDReceiver : public RCSwitchRawReceiver {
 public:
  RCSwitchTypeDReceiver(const std::string &name, RCSwitchProtocol aProtocol, uint8_t group, uint8_t device, bool state);
};

class RCSwitchDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData *data) override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_

#endif //ESPHOMELIB_REMOTE_RC_SWITCH_H
