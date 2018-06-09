//
//  sony.h
//  esphomelib
//
//  Created by Otto Winter on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SONY_H
#define ESPHOMELIB_SONY_H

#include "esphomelib/defines.h"
#include "esphomelib/remote/remote_protocol.h"

#ifdef USE_REMOTE

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

class SonyTransmitter : public RemoteTransmitter {
 public:
  SonyTransmitter(const std::string &name, uint32_t data, uint8_t nbits);

  RemoteTransmitData get_data() override;

 protected:
  uint32_t data_;
  uint8_t nbits_;
};

bool decode_sony(RemoteReceiveData &data, uint32_t *data_, uint8_t *nbits);

class SonyDecoder : public RemoteReceiveDecoder {
 public:
  SonyDecoder(const std::string &name, uint32_t data, uint8_t nbits);

 protected:
  bool matches(RemoteReceiveData &data) override;

 protected:
  uint32_t data_;
  uint8_t nbits_;
};

class SonyDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData &data) override;
};

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_SONY_H
