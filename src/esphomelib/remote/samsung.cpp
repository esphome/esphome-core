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

static const uint32_t NBITS = 32;
static const uint32_t HEADER_HIGH_US = 4500;
static const uint32_t HEADER_LOW_US = 4500;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 1690;
static const uint32_t BIT_ZERO_LOW_US = 560;
static const uint32_t FOOTER_HIGH_US = 560;
static const uint32_t FOOTER_LOW_US = 560;

#ifdef USE_REMOTE_TRANSMITTER
SamsungTransmitter::SamsungTransmitter(const std::string &name, uint32_t data)
    : RemoteTransmitter(name), data_(data) {}

void SamsungTransmitter::to_data(RemoteTransmitData *data) {
  data->set_carrier_frequency(38000);
  data->reserve(4 + NBITS * 2u);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (NBITS - 1); mask != 0; mask >>= 1) {
    if (this->data_ & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data->item(FOOTER_HIGH_US, FOOTER_LOW_US);
}
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_samsung(RemoteReceiveData *data, uint32_t *data_) {
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return false;

  *data_ = 0;
  for (uint8_t i = 0; i < NBITS; i++) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *data_ = (*data_ << 1) | 1;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *data_ = (*data_ << 1) | 0;
    } else {
      return false;
    }
  }

  return data->expect_mark(FOOTER_HIGH_US));
}

SamsungReceiver::SamsungReceiver(const std::string &name, uint32_t data)
    : RemoteReceiver(name), data_(data) {}

bool SamsungReceiver::matches(RemoteReceiveData *data) {
  uint32_t data_;
  if (!decode_samsung(data, &data_))
    return false;

  return this->data_ == data_;
}

void SamsungDumper::dump(RemoteReceiveData *data) {
  uint32_t data_;
  if (!decode_samsung(data, &data_))
    return;

  ESP_LOGD(TAG, "Received Samsung: data=0x%08X", data_);
}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
