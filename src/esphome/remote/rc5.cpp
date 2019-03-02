#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/rc5.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.rc5";
#endif

static const uint32_t BIT_TIME_US = 889;
static const uint8_t NBITS = 14;

#ifdef USE_REMOTE_TRANSMITTER
RC5Transmitter::RC5Transmitter(const std::string &name, uint8_t address, uint8_t command)
    : RemoteTransmitter(name), address_(address), command_(command) {}

void RC5Transmitter::to_data(RemoteTransmitData *data) {
  encode_rc5(data, this->address_, this->command_, this->toggle_);
  this->toggle_ = !this->toggle_;
}

void encode_rc5(RemoteTransmitData *data, uint8_t address, uint8_t command, bool toggle) {
  data->set_carrier_frequency(36000);

  uint64_t out_data = 0;
  out_data |= 0b11 << 12;
  out_data |= toggle << 11;
  out_data |= address << 6;
  out_data |= command;

  for (uint64_t mask = 1UL << (NBITS - 1); mask != 0; mask >>= 1) {
    if (out_data & mask) {
      data->space(BIT_TIME_US);
      data->mark(BIT_TIME_US);
    } else {
      data->mark(BIT_TIME_US);
      data->space(BIT_TIME_US);
    }
  }
}
#endif

#ifdef USE_REMOTE_RECEIVER
RC5DecodeData decode_rc5(RemoteReceiveData *data) {
  RC5DecodeData out{};
  out.valid = false;
  out.address = 0;
  out.command = 0;
  data->expect_space(BIT_TIME_US);
  if (!data->expect_mark(BIT_TIME_US) || !data->expect_space(BIT_TIME_US) || !data->expect_mark(BIT_TIME_US)) {
    return out;
  }

  uint64_t out_data = 0;
  for (int bit = NBITS - 3; bit >= 0; bit--) {
    if (data->expect_space(BIT_TIME_US) && data->expect_mark(BIT_TIME_US)) {
      out_data |= 1 << bit;
    } else if (data->expect_mark(BIT_TIME_US) && data->expect_space(BIT_TIME_US)) {
      out_data |= 0 << bit;
    } else {
      out.valid = false;
      return out;
    }
  }

  out.command = out_data & 0x3F;
  out.address = (out_data >> 6) & 0x1F;
  out.valid = true;
  return out;
}

RC5Receiver::RC5Receiver(const std::string &name, uint8_t address, uint8_t command)
    : RemoteReceiver(name), address_(address), command_(command) {}

bool RC5Receiver::matches(RemoteReceiveData *data) {
  auto res = decode_rc5(data);
  if (!res.valid)
    return false;

  return this->address_ == res.address && this->command_ == res.command;
}

bool RC5Dumper::dump(RemoteReceiveData *data) {
  auto res = decode_rc5(data);
  if (!res.valid)
    return false;

  ESP_LOGD(TAG, "Received RC5: address=0x%02X, command=0x%02X", res.address, res.command);
  return true;
}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
