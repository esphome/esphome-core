//
//  panasonic.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/remote/panasonic.h"
#include "esphomelib/log.h"

#ifdef USE_REMOTE

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.panasonic";

static const uint32_t HEADER_HIGH_US = 3502;
static const uint32_t HEADER_LOW_US = 1750;
static const uint32_t BIT_HIGH_US = 502;
static const uint32_t BIT_ZERO_LOW_US = 400;
static const uint32_t BIT_ONE_LOW_US = 1244;

RemoteTransmitData PanasonicTransmitter::get_data() {
  RemoteTransmitData data{};
  data.reserve(100);
  data.item(HEADER_HIGH_US, HEADER_LOW_US);
  data.set_carrier_frequency(35000);

  uint32_t mask;
  for (mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (this->address_ & mask)
      data.item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data.item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  for (mask = 1UL << 31; mask != 0; mask >>= 1) {
    if (this->data_ & mask)
      data.item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data.item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }
  data.mark(BIT_HIGH_US);
  return data;
}
PanasonicTransmitter::PanasonicTransmitter(const std::string &name,
                                           uint16_t address,
                                           uint32_t data)
    : RemoteTransmitter(name), address_(address), data_(data) {

}

bool decode_panasonic(RemoteReceiveData &data, uint16_t *address, uint32_t *data_) {
  if (!data.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return false;

  *address = 0;
  uint32_t mask;
  for (mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (data.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *address |= mask;
    } else if (data.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *address &= ~mask;
    } else {
      return false;
    }

  }

  *data_ = 0;
  for (mask = 1UL << 31; mask != 0; mask >>= 1) {
    if (data.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *data_ |= mask;
    } else if (data.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *data_ &= ~mask;
    } else {
      return false;
    }
  }

  return true;
}

bool PanasonicDecoder::matches(RemoteReceiveData &data) {
  uint16_t address;
  uint32_t data_;
  if (!decode_panasonic(data, &address, &data_))
    return false;

  return this->address_ == address && this->data_ == data_;
}
PanasonicDecoder::PanasonicDecoder(const std::string &name, uint16_t address, uint32_t data)
    : RemoteReceiveDecoder(name), address_(address), data_(data) {

}

void PanasonicDumper::dump(RemoteReceiveData &data) {
  uint16_t address;
  uint32_t data_;
  if (!decode_panasonic(data, &address, &data_))
    return;

  ESP_LOGD(TAG, "Received Panasonic: address=0x%04X, data=0x%08X", address, data_);
}

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
