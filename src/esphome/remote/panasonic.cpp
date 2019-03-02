#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/panasonic.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

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
void PanasonicTransmitter::to_data(RemoteTransmitData *data) { encode_panasonic(data, this->address_, this->command_); }

PanasonicTransmitter::PanasonicTransmitter(const std::string &name, uint16_t address, uint32_t command)
    : RemoteTransmitter(name), address_(address), command_(command) {}

void encode_panasonic(RemoteTransmitData *data, uint16_t address, uint32_t command) {
  data->reserve(100);
  data->item(HEADER_HIGH_US, HEADER_LOW_US);
  data->set_carrier_frequency(35000);

  uint32_t mask;
  for (mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (address & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  for (mask = 1UL << 31; mask != 0; mask >>= 1) {
    if (command & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }
  data->mark(BIT_HIGH_US);
}
#endif

#ifdef USE_REMOTE_RECEIVER
PanasonicDecodeData decode_panasonic(RemoteReceiveData *data) {
  PanasonicDecodeData out{};
  out.valid = false;
  out.address = 0;
  out.command = 0;
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return out;

  uint32_t mask;
  for (mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      out.address |= mask;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      out.address &= ~mask;
    } else {
      return out;
    }
  }

  for (mask = 1UL << 31; mask != 0; mask >>= 1) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      out.command |= mask;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      out.command &= ~mask;
    } else {
      return out;
    }
  }

  out.valid = true;
  return out;
}

bool PanasonicReceiver::matches(RemoteReceiveData *data) {
  auto decode = decode_panasonic(data);

  return decode.valid && this->address_ == decode.address && this->command_ == decode.command;
}
PanasonicReceiver::PanasonicReceiver(const std::string &name, uint16_t address, uint32_t command)
    : RemoteReceiver(name), address_(address), command_(command) {}

bool PanasonicDumper::dump(RemoteReceiveData *data) {
  auto decode = decode_panasonic(data);
  if (!decode.valid)
    return false;

  ESP_LOGD(TAG, "Received Panasonic: address=0x%04X, command=0x%08X", decode.address, decode.command);
  return true;
}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
