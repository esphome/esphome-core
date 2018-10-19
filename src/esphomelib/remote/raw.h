#ifndef ESPHOMELIB_REMOTE_RAW_H
#define ESPHOMELIB_REMOTE_RAW_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_receiver.h"
#include "esphomelib/remote/remote_transmitter.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class RawTransmitter : public RemoteTransmitter {
 public:
  RawTransmitter(const std::string &name, std::vector<int32_t> data,
                 uint32_t carrier_frequency = 0);

  void to_data(RemoteTransmitData *data) override;

 protected:
  std::vector<int32_t> data_;
  uint32_t carrier_frequency_{0};
};
#endif

#ifdef USE_REMOTE_RECEIVER
class RawReceiver : public RemoteReceiver {
 public:
  RawReceiver(const std::string &name, std::vector<int32_t> data);

  bool matches(RemoteReceiveData *data) override;

 protected:
  std::vector<int32_t> data_;
};

class RawDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData *data) override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_RAW_H
