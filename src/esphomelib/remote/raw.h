//
//  raw.h
//  esphomelib
//
//  Created by Otto Winter on 05.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_RAW_H
#define ESPHOMELIB_RAW_H

#include "esphomelib/remote/remote_protocol.h"
#include "esphomelib/defines.h"

#ifdef USE_REMOTE

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

class RawTransmitter : public RemoteTransmitter {
 public:
  RawTransmitter(const std::string &name, const std::vector<int32_t> data,
                 uint32_t carrier_frequency);
  RemoteTransmitData get_data() override;

 protected:
  std::vector<int32_t> data_;
  uint32_t carrier_frequency_{0};
};

class RawDecoder : public RemoteReceiveDecoder {
 public:
  RawDecoder(const std::string &name, std::vector<int32_t> data);

  bool matches(RemoteReceiveData &data) override;

 protected:
  std::vector<int32_t> data_;
};

class RawDumper : public RemoteReceiveDumper {
 public:
  void dump(RemoteReceiveData &data) override;
};

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_RAW_H
