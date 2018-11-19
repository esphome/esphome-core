#ifndef ESPHOMELIB_REMOTE_NEC_H
#define ESPHOMELIB_REMOTE_NEC_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_receiver.h"
#include "esphomelib/remote/remote_transmitter.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class NECTransmitter : public RemoteTransmitter {
 public:
  NECTransmitter(const std::string &name, uint16_t address, uint16_t command);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint16_t address_;
  uint16_t command_;
};

void encode_nec(RemoteTransmitData *data, uint16_t address, uint16_t command);
#endif

#ifdef USE_REMOTE_RECEIVER
NECDecodeData decode_nec(RemoteReceiveData *data);

class NECReceiver : public RemoteReceiver {
 public:
  NECReceiver(const std::string &name, uint16_t address, uint16_t command);

  bool matches(RemoteReceiveData *data) override;

 protected:
  uint16_t address_;
  uint16_t command_;
};

class NECDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData *data) override;
};
#endif

}  // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif  // USE_REMOTE

#endif  // ESPHOMELIB_REMOTE_NEC_H
