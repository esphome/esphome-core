#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/jvc.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.jvc";

static const uint8_t NBITS = 16;
static const uint32_t HEADER_HIGH_US = 8400;
static const uint32_t HEADER_LOW_US = 4200;
static const uint32_t BIT_ONE_LOW_US = 1725;
static const uint32_t BIT_ZERO_LOW_US = 525;
static const uint32_t BIT_HIGH_US = 525;

#ifdef USE_REMOTE_TRANSMITTER
JVCTransmitter::JVCTransmitter(const std::string &name, uint32_t data) : RemoteTransmitter(name), data_(data) {}

void JVCTransmitter::to_data(RemoteTransmitData *data) { encode_jvc(data, this->data_); }

void encode_jvc(RemoteTransmitData *data, uint32_t jvc_data) {
  data->set_carrier_frequency(38000);
  data->reserve(2 + NBITS * 2u);

  data->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (NBITS - 1); mask != 0; mask >>= 1) {
    if (jvc_data & mask)
      data->item(BIT_HIGH_US, BIT_ONE_LOW_US);
    else
      data->item(BIT_HIGH_US, BIT_ZERO_LOW_US);
  }

  data->mark(BIT_HIGH_US);
}
#endif

#ifdef USE_REMOTE_RECEIVER
JVCDecodeData decode_jvc(RemoteReceiveData *data) {
  JVCDecodeData out{};
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

  out.valid = true;
  return out;
}

JVCReceiver::JVCReceiver(const std::string &name, uint32_t data) : RemoteReceiver(name), data_(data) {}

bool JVCReceiver::matches(RemoteReceiveData *data) {
  auto decode = decode_jvc(data);
  if (!decode.valid)
    return false;

  return this->data_ == decode.data;
}

bool JVCDumper::dump(RemoteReceiveData *data) {
  auto decode = decode_jvc(data);
  if (!decode.valid)
    return false;

  ESP_LOGD(TAG, "Received JVC: data=0x%04X", decode.data);
  return true;
}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
