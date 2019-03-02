#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/nec.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.nec";
#endif

static const uint32_t HEADER_HIGH_US = 9000;
static const uint32_t HEADER_LOW_US = 4500;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 1690;
static const uint32_t BIT_ZERO_LOW_US = 560;

#ifdef USE_REMOTE_TRANSMITTER
void encode_nec(RemoteTransmitData *data, uint16_t address, uint16_t command) {
  data->reserve(68);
  data->set_carrier_frequency(38000);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);
  for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
    if (address & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  for (uint32_t mask = 1UL << 15; mask; mask >>= 1) {
    if (command & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data->mark(BIT_HIGH_US);
}
NECTransmitter::NECTransmitter(const std::string &name, uint16_t address, uint16_t command)
    : RemoteTransmitter(name), address_(address), command_(command) {}
void NECTransmitter::to_data(RemoteTransmitData *data) { encode_nec(data, this->address_, this->command_); }
#endif

#ifdef USE_REMOTE_RECEIVER
NECDecodeData decode_nec(RemoteReceiveData *data) {
  NECDecodeData out{};
  out.valid = false;
  out.address = 0;
  out.command = 0;
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return out;

  for (uint32_t mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      out.address |= mask;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      out.address &= ~mask;
    } else {
      return out;
    }
  }

  for (uint32_t mask = 1UL << 15; mask != 0; mask >>= 1) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      out.command |= mask;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      out.command &= ~mask;
    } else {
      return out;
    }
  }

  data->expect_mark(BIT_HIGH_US);
  out.valid = true;
  return out;
}

NECReceiver::NECReceiver(const std::string &name, uint16_t address, uint16_t command)
    : RemoteReceiver(name), address_(address), command_(command) {}
bool NECReceiver::matches(RemoteReceiveData *data) {
  auto decode = decode_nec(data);
  if (!decode.valid)
    return false;

  return this->address_ == decode.address && this->command_ == decode.command;
}
bool NECDumper::dump(RemoteReceiveData *data) {
  auto decode = decode_nec(data);
  if (!decode.valid)
    return false;

  ESP_LOGD(TAG, "Received NEC: address=0x%04X, command=0x%04X", decode.address, decode.command);
  return true;
}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
