//
//  samsung.cpp
//  esphomelib
//
//  Created by Otto Winter on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/samsung.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.samsung";
#endif

static const uint32_t HEADER_HIGH_US = 4500;
static const uint32_t HEADER_LOW_US = 4500;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 560;
static const uint32_t BIT_ZERO_LOW_US = 1690;

#ifdef USE_REMOTE_TRANSMITTER
SamsungTransmitter::SamsungTransmitter(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteTransmitter(name), data_(data), nbits_(nbits) {}

void SamsungTransmitter::to_data(RemoteTransmitData *data) {
  data->set_carrier_frequency(38000);
  data->reserve(2 + this->nbits_ * 2u);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (this->nbits_ - 1); mask != 0; mask >>= 1) {
    if (this->data_ & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data->mark(BIT_HIGH_US);
}
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_samsung(RemoteReceiveData *data, uint32_t *data_, uint8_t *nbits) {
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return false;

  *data_ = 0;
  for (*nbits = 0; *nbits < 32; (*nbits)++) {
    ESP_LOGD(TAG, "Decode Samsung: data=0x%08X, nbits=%d", *data_, *nbits);
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *data_ = (*data_ << 1) | 1;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *data_ = (*data_ << 1) | 0;
    } else {
      return *nbits == 28;
    }
  }
  return true;
}

SamsungReceiver::SamsungReceiver(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteReceiver(name), data_(data), nbits_(nbits) {}

bool SamsungReceiver::matches(RemoteReceiveData *data) {
  uint32_t data_;
  uint8_t nbits;
  if (!decode_samsung(data, &data_, &nbits))
    return false;

  return this->data_ == data_ && this->nbits_ == nbits;
}

void SamsungDumper::dump(RemoteReceiveData *data) {
  uint32_t data_;
  uint8_t nbits;
  if (!decode_samsung(data, &data_, &nbits))
    return;

  ESP_LOGD(TAG, "Received Samsung: data=0x%08X, nbits=%d", data_, nbits);
}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
