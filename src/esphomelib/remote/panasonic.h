#ifndef ESPHOMELIB_REMOTE_PANASONIC_H
#define ESPHOMELIB_REMOTE_PANASONIC_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_receiver.h"
#include "esphomelib/remote/remote_transmitter.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class PanasonicTransmitter : public RemoteTransmitter {
 public:
  PanasonicTransmitter(const std::string &name, uint16_t address, uint32_t command);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint16_t address_;
  uint32_t command_;
};

void encode_panasonic(RemoteTransmitData *data, uint16_t address, uint32_t command);
#endif

#ifdef USE_REMOTE_RECEIVER
PanasonicDecodeData decode_panasonic(RemoteReceiveData *data);

class PanasonicReceiver : public RemoteReceiver {
 public:
  PanasonicReceiver(const std::string &name, uint16_t address, uint32_t command);

  bool matches(RemoteReceiveData *data) override;

 protected:
  uint16_t address_;
  uint32_t command_;
};

class PanasonicDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData *data) override;
};
#endif

}  // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif  // USE_REMOTE

#endif  // ESPHOMELIB_REMOTE_PANASONIC_H
