//
//  lg.h
//  esphomelib
//
//  Created by Otto Winter on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_REMOTE_LG_H
#define ESPHOMELIB_REMOTE_LG_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_protocol.h"

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
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_lg(RemoteReceiveData *data, uint32_t *data_, uint8_t *nbits);

class LGReceiver : public RemoteReceiver {
 public:
  LGReceiver(const std::string &name, uint32_t data, uint8_t nbits);

 protected:
  bool matches(RemoteReceiveData *data) override;

 protected:
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
