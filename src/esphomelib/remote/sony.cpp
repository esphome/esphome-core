//
//  sony.cpp
//  esphomelib
//
//  Created by Otto Winter on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/sony.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.sony";

static const uint32_t HEADER_HIGH_US = 2400;
static const uint32_t HEADER_LOW_US = 600;
static const uint32_t BIT_ONE_HIGH_US = 1200;
static const uint32_t BIT_ZERO_HIGH_US = 600;
static const uint32_t BIT_LOW_US = 600;

#ifdef USE_REMOTE_TRANSMITTER
SonyTransmitter::SonyTransmitter(const std::string &name,
                                 uint32_t data,
                                 uint8_t nbits)
    : RemoteTransmitter(name), data_(data), nbits_(nbits) {

}
void SonyTransmitter::to_data(RemoteTransmitData *data) {
  data->set_carrier_frequency(40000);
  data->reserve(2 + this->nbits_ * 2u);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (this->nbits_ - 1); mask != 0; mask >>= 1) {
    if (this->data_ & mask)
      data->item(BIT_ONE_HIGH_US, BIT_LOW_US);
    else
      data->item(BIT_ZERO_HIGH_US, BIT_LOW_US);
  }
}
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_sony(RemoteReceiveData *data, uint32_t *data_, uint8_t *nbits) {
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return false;

  *data_ = 0;
  for (*nbits = 0; *nbits < 20; (*nbits)++) {
    uint32_t bit;
    if (data->expect_mark(BIT_ONE_HIGH_US)) {
      bit = 1;
    } else if (data->expect_mark(BIT_ZERO_HIGH_US)) {
      bit = 0;
    } else {
      return *nbits == 12 || *nbits == 15;
    }

    *data_ = (*data_ << 1) | bit;
    if (data->expect_space(BIT_LOW_US)) {
      // nothing needs to be done
    } else if (data->peek_space_at_least(BIT_LOW_US)) {
      *nbits += 1;
      return *nbits == 12 || *nbits == 15 || *nbits == 20;
    } else {
      return false;
    }
  }
  return true;
}

SonyReceiver::SonyReceiver(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteReceiver(name), data_(data), nbits_(nbits) {}

bool SonyReceiver::matches(RemoteReceiveData *data) {
  uint32_t data_;
  uint8_t nbits;
  if (!decode_sony(data, &data_, &nbits))
    return false;

  return this->data_ == data_ && this->nbits_ == nbits;
}

void SonyDumper::dump(RemoteReceiveData *data) {
  uint32_t data_;
  uint8_t nbits;
  if (!decode_sony(data, &data_, &nbits))
    return;

  ESP_LOGD(TAG, "Received Sony: data=0x%08X, nbits=%d", data_, nbits);
}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
