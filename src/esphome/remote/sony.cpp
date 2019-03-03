#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/sony.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.sony";

static const uint32_t HEADER_HIGH_US = 2400;
static const uint32_t HEADER_LOW_US = 600;
static const uint32_t BIT_ONE_HIGH_US = 1200;
static const uint32_t BIT_ZERO_HIGH_US = 600;
static const uint32_t BIT_LOW_US = 600;

#ifdef USE_REMOTE_TRANSMITTER
SonyTransmitter::SonyTransmitter(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteTransmitter(name), data_(data), nbits_(nbits) {}
void SonyTransmitter::to_data(RemoteTransmitData *data) { encode_sony(data, this->data_, this->nbits_); }

void encode_sony(RemoteTransmitData *data, uint32_t sony_data, uint8_t nbits) {
  data->set_carrier_frequency(40000);
  data->reserve(2 + nbits * 2u);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (nbits - 1); mask != 0; mask >>= 1) {
    if (sony_data & mask)
      data->item(BIT_ONE_HIGH_US, BIT_LOW_US);
    else
      data->item(BIT_ZERO_HIGH_US, BIT_LOW_US);
  }
}
#endif

#ifdef USE_REMOTE_RECEIVER
SonyDecodeData decode_sony(RemoteReceiveData *data) {
  SonyDecodeData out{};
  out.valid = false;
  out.data = 0;
  out.nbits = 0;
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return out;

  for (; out.nbits < 20; out.nbits++) {
    uint32_t bit;
    if (data->expect_mark(BIT_ONE_HIGH_US)) {
      bit = 1;
    } else if (data->expect_mark(BIT_ZERO_HIGH_US)) {
      bit = 0;
    } else {
      out.valid = out.nbits == 12 || out.nbits == 15;
      return out;
    }

    out.data = (out.data << 1UL) | bit;
    if (data->expect_space(BIT_LOW_US)) {
      // nothing needs to be done
    } else if (data->peek_space_at_least(BIT_LOW_US)) {
      out.nbits += 1;
      out.valid = out.nbits == 12 || out.nbits == 15 || out.nbits == 20;
      return out;
    } else {
      return out;
    }
  }

  out.valid = true;
  return out;
}

SonyReceiver::SonyReceiver(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteReceiver(name), data_(data), nbits_(nbits) {}

bool SonyReceiver::matches(RemoteReceiveData *data) {
  auto decode = decode_sony(data);
  return decode.valid && this->data_ == decode.data && this->nbits_ == decode.nbits;
}

bool SonyDumper::dump(RemoteReceiveData *data) {
  auto decode = decode_sony(data);
  if (!decode.valid)
    return false;

  ESP_LOGD(TAG, "Received Sony: data=0x%08X, nbits=%d", decode.data, decode.nbits);
  return true;
}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
