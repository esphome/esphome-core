#ifndef ESPHOME_REMOTE_LG_H
#define ESPHOME_REMOTE_LG_H

#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/remote_receiver.h"
#include "esphome/remote/remote_transmitter.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class LGTransmitter : public RemoteTransmitter {
 public:
  LGTransmitter(const std::string &name, uint32_t data, uint8_t nbits);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint32_t data_;
  uint8_t nbits_;
};

void encode_lg(RemoteTransmitData *data, uint32_t lg_data, uint8_t nbits);
#endif

#ifdef USE_REMOTE_RECEIVER
LGDecodeData decode_lg(RemoteReceiveData *data);

class LGReceiver : public RemoteReceiver {
 public:
  LGReceiver(const std::string &name, uint32_t data, uint8_t nbits);

 protected:
  bool matches(RemoteReceiveData *data) override;

  uint32_t data_;
  uint8_t nbits_;
};

class LGDumper : public RemoteReceiveDumper {
 public:
  bool dump(RemoteReceiveData *data) override;
};
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE

#endif  // ESPHOME_REMOTE_LG_H
