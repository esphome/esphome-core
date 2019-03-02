#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/lg.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.lg";
#endif

static const uint32_t HEADER_HIGH_US = 8000;
static const uint32_t HEADER_LOW_US = 4000;
static const uint32_t BIT_HIGH_US = 600;
static const uint32_t BIT_ONE_LOW_US = 1600;
static const uint32_t BIT_ZERO_LOW_US = 550;

#ifdef USE_REMOTE_TRANSMITTER
LGTransmitter::LGTransmitter(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteTransmitter(name), data_(data), nbits_(nbits) {}

void LGTransmitter::to_data(RemoteTransmitData *data) {
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

void encode_lg(RemoteTransmitData *data, uint32_t lg_data, uint8_t nbits) {
  data->set_carrier_frequency(38000);
  data->reserve(2 + nbits * 2u);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (nbits - 1); mask != 0; mask >>= 1) {
    if (lg_data & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data->mark(BIT_HIGH_US);
}
#endif

#ifdef USE_REMOTE_RECEIVER
LGDecodeData decode_lg(RemoteReceiveData *data) {
  LGDecodeData out{};
  out.valid = false;
  out.data = 0;
  out.nbits = 0;
  if (!data->expect_item(HEADER_HIGH_US, HEADER_LOW_US)) {
    return out;
  }

  for (out.nbits = 0; out.nbits < 32; out.nbits++) {
    if (data->expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      out.data = (out.data << 1) | 1;
    } else if (data->expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      out.data = (out.data << 1) | 0;
    } else {
      out.valid = out.nbits == 28;
      return out;
    }
  }

  out.valid = true;
  return out;
}

LGReceiver::LGReceiver(const std::string &name, uint32_t data, uint8_t nbits)
    : RemoteReceiver(name), data_(data), nbits_(nbits) {}

bool LGReceiver::matches(RemoteReceiveData *data) {
  auto res = decode_lg(data);
  if (!res.valid)
    return false;

  return this->data_ == res.data && this->nbits_ == res.nbits;
}

bool LGDumper::dump(RemoteReceiveData *data) {
  auto res = decode_lg(data);
  if (!res.valid)
    return false;

  ESP_LOGD(TAG, "Received LG: data=0x%08X, nbits=%d", res.data, res.nbits);
  return true;
}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
