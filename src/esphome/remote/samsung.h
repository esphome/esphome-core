#ifndef ESPHOME_REMOTE_SAMSUNG_H
#define ESPHOME_REMOTE_SAMSUNG_H

#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/remote_receiver.h"
#include "esphome/remote/remote_transmitter.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class SamsungTransmitter : public RemoteTransmitter {
 public:
  SamsungTransmitter(const std::string &name, uint32_t data);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint32_t data_;
};

void encode_samsung(RemoteTransmitData *data, uint32_t samsung_data);
#endif

#ifdef USE_REMOTE_RECEIVER
SamsungDecodeData decode_samsung(RemoteReceiveData *data);

class SamsungReceiver : public RemoteReceiver {
 public:
  SamsungReceiver(const std::string &name, uint32_t data);

 protected:
  bool matches(RemoteReceiveData *data) override;

  uint32_t data_;
};

class SamsungDumper : public RemoteReceiveDumper {
 public:
  bool dump(RemoteReceiveData *data) override;
};
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE

#endif  // ESPHOME_REMOTE_SAMSUNG_H
