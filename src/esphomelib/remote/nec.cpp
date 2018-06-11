//
//  nec.cpp
//  esphomelib
//
//  Created by Otto Winter on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/remote/nec.h"
#include "esphomelib/log.h"

#ifdef USE_REMOTE

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.nec";

static const uint32_t HEADER_HIGH_US = 9000;
static const uint32_t HEADER_LOW_US = 4500;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 1690;
static const uint32_t BIT_ZERO_LOW_US = 560;

#ifdef USE_REMOTE_TRANSMITTER
RemoteTransmitData NECTransmitter::get_data() {
  RemoteTransmitData data{};
  data.reserve(68);
  data.set_carrier_frequency(38000);

  data.item(HEADER_HIGH_US, HEADER_LOW_US);
  for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
    if (this->address_ & mask)
      data.item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data.item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
    if (this->command_ & mask)
      data.item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data.item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data.mark(BIT_HIGH_US);
  return data;
}
NECTransmitter::NECTransmitter(const std::string &name,
                               uint16_t address,
                               uint16_t command)
    : RemoteTransmitter(name),  address_(address), command_(command) {

}
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_nec(RemoteReceiveData &data, uint16_t *address, uint16_t *command) {
  if (!data.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return false;

  *address = 0;
  for (uint32_t mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (data.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *address |= mask;
    } else if (data.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *address &= ~mask;
    } else {
      return false;
    }
  }

  *command = 0;
  for (uint32_t mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (data.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *address |= mask;
    } else if (data.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *address &= ~mask;
    } else {
      return false;
    }
  }

  data.expect_mark(BIT_HIGH_US);
  return true;
}

NECDecoder::NECDecoder(const std::string &name, uint16_t address, uint16_t command)
    : RemoteReceiveDecoder(name), address_(address), command_(command) {

}
bool NECDecoder::matches(RemoteReceiveData &data) {
  uint16_t address;
  uint16_t command;
  if (!decode_nec(data, &address, &command))
    return false;

  return this->address_ == address && this->command_ == command;
}
void NECDumper::dump(RemoteReceiveData &data) {
  uint16_t address;
  uint16_t command;
  if (!decode_nec(data, &address, &command))
    return;

  ESP_LOGD(TAG, "Received NEC: address=0x%04X, command=0x%04X", address, command);
}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
