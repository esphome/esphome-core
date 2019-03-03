#ifndef ESPHOME_RC_SWITCH_PROTOCOL_H
#define ESPHOME_RC_SWITCH_PROTOCOL_H

#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/component.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

class RemoteTransmitData;
class RemoteReceiveData;

class RCSwitchProtocol {
 public:
  RCSwitchProtocol(uint32_t sync_high, uint32_t sync_low, uint32_t zero_high, uint32_t zero_low, uint32_t one_high,
                   uint32_t one_low, bool inverted);

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

  static void type_a_code(uint8_t switch_group, uint8_t switch_device, bool state, uint32_t *out_code,
                          uint8_t *out_nbits);

  static void type_b_code(uint8_t address_code, uint8_t channel_code, bool state, uint32_t *out_code,
                          uint8_t *out_nbits);

  static void type_c_code(uint8_t family, uint8_t group, uint8_t device, bool state, uint32_t *out_code,
                          uint8_t *out_nbits);

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

}  // namespace remote

ESPHOME_NAMESPACE_END

#include "esphome/remote/remote_receiver.h"
#include "esphome/remote/remote_transmitter.h"

#endif  // USE_REMOTE

#endif  // ESPHOME_RC_SWITCH_PROTOCOL_H
