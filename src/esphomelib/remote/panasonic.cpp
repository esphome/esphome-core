#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/panasonic.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.panasonic";
#endif

static const uint32_t HEADER_HIGH_US = 3502;
static const uint32_t HEADER_LOW_US = 1750;
static const uint32_t BIT_HIGH_US = 502;
static const uint32_t BIT_ZERO_LOW_US = 400;
static const uint32_t BIT_ONE_LOW_US = 1244;

#ifdef USE_REMOTE_TRANSMITTER
void PanasonicTransmitter::to_data(RemoteTransmitData *data) {
  data->reserve(100);
  data->item(HEADER_HIGH_US, HEADER_LOW_US);
  data->set_carrier_frequency(35000);

  uint32_t mask;
  for (mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (this->address_ & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  for (mask = 1UL << 31; mask != 0; mask >>= 1) {
    if (this->command_ & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }
  data->mark(BIT_HIGH_US);
}

PanasonicTransmitter::PanasonicTransmitter(const std::string &name,
                                           uint16_t address,
                                           uint32_t command)
    : RemoteTransmitter(name), address_(address), command_(command) {

}
#endif

#ifdef USE_REMOTE_RECEIVER
bool decode_panasonic(RemoteReceiveData *data, uint16_t *address, uint32_t *command) {
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return false;

  *address = 0;
  uint32_t mask;
  for (mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *address |= mask;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *address &= ~mask;
    } else {
      return false;
    }

  }

  *command = 0;
  for (mask = 1UL << 31; mask != 0; mask >>= 1) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      *command |= mask;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      *command &= ~mask;
    } else {
      return false;
    }
  }

  return true;
}

bool PanasonicReceiver::matches(RemoteReceiveData *data) {
  uint16_t address;
  uint32_t command;
  if (!decode_panasonic(data, &address, &command))
    return false;

  return this->address_ == address && this->command_ == command;
}
PanasonicReceiver::PanasonicReceiver(const std::string &name, uint16_t address, uint32_t command)
    : RemoteReceiver(name), address_(address), command_(command) {

}

void PanasonicDumper::dump(RemoteReceiveData *data) {
  uint16_t address;
  uint32_t command;
  if (!decode_panasonic(data, &address, &command))
    return;

  ESP_LOGD(TAG, "Received Panasonic: address=0x%04X, command=0x%08X", address, command);
}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
