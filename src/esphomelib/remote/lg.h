#ifndef ESPHOMELIB_REMOTE_LG_H
#define ESPHOMELIB_REMOTE_LG_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_receiver.h"
#include "esphomelib/remote/remote_transmitter.h"

ESPHOMELIB_NAMESPACE_BEGIN

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

void encode_lg(RemoteTransmitData *data, uint32_t data_, uint8_t nbits);
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
  void dump(RemoteReceiveData *data) override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_LG_H
