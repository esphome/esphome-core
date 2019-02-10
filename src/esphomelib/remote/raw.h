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
  RawTransmitter(const std::string &name, const int32_t *data, size_t len,
                 uint32_t carrier_frequency = 0);

  void to_data(RemoteTransmitData *data) override;

 protected:
  const int32_t *data_;
  size_t len_;
  uint32_t carrier_frequency_{0};
};
#endif

#ifdef USE_REMOTE_RECEIVER
class RawReceiver : public RemoteReceiver {
 public:
  RawReceiver(const std::string &name, const int32_t *data, size_t len);

  bool matches(RemoteReceiveData *data) override;

 protected:
  const int32_t *data_;
  size_t len_;
};

class RawDumper : public RemoteReceiveDumper {
 public:
  bool dump(RemoteReceiveData *data) override;

  bool secondary_() override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_RAW_H
