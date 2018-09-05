//
//  samsung.h
//  esphomelib
//
//  Created by Otto Winter on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

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
  SamsungTransmitter(const std::string &name, uint32_t data, uint8_t nbits);

  void to_data(RemoteTransmitData *data) override;

 protected:
  uint32_t data_;
  uint8_t nbits_;
};
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_samsung(RemoteReceiveData *data, uint32_t *data_, uint8_t *nbits);

class SamsungReceiver : public RemoteReceiver {
 public:
  SamsungReceiver(const std::string &name, uint32_t data, uint8_t nbits);

 protected:
  bool matches(RemoteReceiveData *data) override;

 protected:
  uint32_t data_;
  uint8_t nbits_;
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
