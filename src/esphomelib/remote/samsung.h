#ifndef ESPHOMELIB_REMOTE_SAMSUNG_H
#define ESPHOMELIB_REMOTE_SAMSUNG_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_protocol.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class SamsungTransmitter : public RemoteTransmitter {
 public:
  SamsungTransmitter(const std::string &name, uint32_t data);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint32_t data_;
};
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_samsung(RemoteReceiveData *data, uint32_t *data_);

class SamsungReceiver : public RemoteReceiver {
 public:
  SamsungReceiver(const std::string &name, uint32_t data);

 protected:
  bool matches(RemoteReceiveData *data) override;

 protected:
  uint32_t data_;
};

class SamsungDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData *data) override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_SAMSUNG_H
