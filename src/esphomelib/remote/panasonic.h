//
//  panasonic.h
//  esphomelib
//
//  Created by Otto Winter on 05.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_PANASONIC_H
#define ESPHOMELIB_PANASONIC_H

#include "esphomelib/remote/remote_protocol.h"
#include "esphomelib/defines.h"

#ifdef USE_REMOTE

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_TRANSMITTER
class PanasonicTransmitter : public RemoteTransmitter {
 public:
  PanasonicTransmitter(const std::string &name, uint16_t address, uint32_t command);

  RemoteTransmitData get_data() override;

 protected:
  uint16_t address_;
  uint32_t command_;
};
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_panasonic(RemoteReceiveData &data, uint16_t *address, uint32_t *command);

class PanasonicDecoder : public RemoteReceiveDecoder {
 public:
  PanasonicDecoder(const std::string &name, uint16_t address, uint32_t command);

  bool matches(RemoteReceiveData &data) override;

 protected:
  uint16_t address_;
  uint32_t command_;
};

class PanasonicDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData &data) override;
};
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_PANASONIC_H
