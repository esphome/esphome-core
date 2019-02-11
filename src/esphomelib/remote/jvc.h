#ifndef ESPHOMELIB_REMOTE_JVC_H
#define ESPHOMELIB_REMOTE_JVC_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_receiver.h"
#include "esphomelib/remote/remote_transmitter.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class JVCTransmitter : public RemoteTransmitter {
 public:
  JVCTransmitter(const std::string &name, uint32_t data);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint32_t data_;
};

void encode_jvc(RemoteTransmitData *data, uint32_t data_);
#endif

#ifdef USE_REMOTE_RECEIVER
JVCDecodeData decode_jvc(RemoteReceiveData *data);

class JVCReceiver : public RemoteReceiver {
 public:
  JVCReceiver(const std::string &name, uint32_t data);

 protected:
  bool matches(RemoteReceiveData *data) override;

 protected:
  uint32_t data_;
};

class JVCDumper : public RemoteReceiveDumper {
 public:
  bool dump(RemoteReceiveData *data) override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_JVC_H
