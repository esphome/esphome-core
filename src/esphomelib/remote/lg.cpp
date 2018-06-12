//
//  lg.cpp
//  esphomelib
//
//  Created by Otto Winter on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/remote/lg.h"
#include "esphomelib/log.h"

#ifdef USE_REMOTE

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.lg";

static const uint32_t HEADER_HIGH_US = 8000;
static const uint32_t HEADER_LOW_US = 4000;
static const uint32_t BIT_HIGH_US = 600;
static const uint32_t BIT_ONE_LOW_US = 1600;
static const uint32_t BIT_ZERO_LOW_US = 550;

#ifdef USE_REMOTE_TRANSMITTER
LGTransmitter::LGTransmitter(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteTransmitter(name), data_(data), nbits_(nbits) {}

RemoteTransmitData LGTransmitter::get_data() {
  RemoteTransmitData data{};
  data.set_carrier_frequency(38000);
  data.reserve(2 + this->nbits_ * 2u);

  data.item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (this->nbits_ - 1); mask != 0; mask >>= 1) {
    if (this->data_ & mask)
      data.item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data.item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data.mark(BIT_HIGH_US);

  return data;
}
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_lg(RemoteReceiveData &data, uint32_t *data_, uint8_t *nbits) {
  if (!data.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return false;

  *data_ = 0;
  for (*nbits = 0; *nbits < 32; (*nbits)++) {
    if (data.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *data_ = (*data_ << 1) | 1;
    } else if (data.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *data_ = (*data_ << 1) | 0;
    } else {
      return *nbits == 28;
    }
  }
  return true;
}

LGReceiver::LGReceiver(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteReceiver(name), data_(data), nbits_(nbits) {}

bool LGReceiver::matches(RemoteReceiveData &data) {
  uint32_t data_;
  uint8_t nbits;
  if (!decode_lg(data, &data_, &nbits))
    return false;

  return this->data_ == data_ && this->nbits_ == nbits;
}

void LGDumper::dump(RemoteReceiveData &data) {
  uint32_t data_;
  uint8_t nbits;
  if (!decode_lg(data, &data_, &nbits))
    return;

  ESP_LOGD(TAG, "Received LG: data=0x%08X, nbits=%d", data_, nbits);
}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
