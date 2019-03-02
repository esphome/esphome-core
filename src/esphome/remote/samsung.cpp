#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/samsung.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.samsung";
#endif

static const uint8_t NBITS = 32;
static const uint32_t HEADER_HIGH_US = 4500;
static const uint32_t HEADER_LOW_US = 4500;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 1690;
static const uint32_t BIT_ZERO_LOW_US = 560;
static const uint32_t FOOTER_HIGH_US = 560;
static const uint32_t FOOTER_LOW_US = 560;

#ifdef USE_REMOTE_TRANSMITTER
SamsungTransmitter::SamsungTransmitter(const std::string &name, uint32_t data) : RemoteTransmitter(name), data_(data) {}

void SamsungTransmitter::to_data(RemoteTransmitData *data) { encode_samsung(data, this->data_); }

void encode_samsung(RemoteTransmitData *data, uint32_t samsung_data) {
  data->set_carrier_frequency(38000);
  data->reserve(4 + NBITS * 2u);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (NBITS - 1); mask != 0; mask >>= 1) {
    if (samsung_data & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data->item(FOOTER_HIGH_US, FOOTER_LOW_US);
}
#endif

#ifdef USE_REMOTE_RECEIVER
SamsungDecodeData decode_samsung(RemoteReceiveData *data) {
  SamsungDecodeData out{};
  out.valid = false;
  out.data = 0;
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return out;

  for (uint8_t i = 0; i < NBITS; i++) {
    out.data <<= 1UL;
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      out.data |= 1UL;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      out.data |= 0UL;
    } else {
      return out;
    }
  }

  out.valid = data->expect_mark(FOOTER_HIGH_US);
  return out;
}

SamsungReceiver::SamsungReceiver(const std::string &name, uint32_t data) : RemoteReceiver(name), data_(data) {}

bool SamsungReceiver::matches(RemoteReceiveData *data) {
  auto decode = decode_samsung(data);
  if (!decode.valid)
    return false;

  return this->data_ == decode.data;
}

bool SamsungDumper::dump(RemoteReceiveData *data) {
  auto decode = decode_samsung(data);
  if (!decode.valid)
    return false;

  ESP_LOGD(TAG, "Received Samsung: data=0x%08X", decode.data);
  return true;
}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
