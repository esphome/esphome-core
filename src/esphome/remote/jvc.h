#ifndef ESPHOME_REMOTE_JVC_H
#define ESPHOME_REMOTE_JVC_H

#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/remote_receiver.h"
#include "esphome/remote/remote_transmitter.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class JVCTransmitter : public RemoteTransmitter {
 public:
  JVCTransmitter(const std::string &name, uint32_t data);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint32_t data_;
};

void encode_jvc(RemoteTransmitData *data, uint32_t jvc_data);
#endif

#ifdef USE_REMOTE_RECEIVER
JVCDecodeData decode_jvc(RemoteReceiveData *data);

class JVCReceiver : public RemoteReceiver {
 public:
  JVCReceiver(const std::string &name, uint32_t data);

 protected:
  bool matches(RemoteReceiveData *data) override;

  uint32_t data_;
};

class JVCDumper : public RemoteReceiveDumper {
 public:
  bool dump(RemoteReceiveData *data) override;
};
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE

#endif  // ESPHOME_REMOTE_JVC_H
