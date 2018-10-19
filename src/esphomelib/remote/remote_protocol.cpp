#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include "esphomelib/remote/remote_protocol.h"
#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.base";

RemoteControlComponentBase::RemoteControlComponentBase(GPIOPin *pin)
    : pin_(pin) {
#ifdef ARDUINO_ARCH_ESP32
  this->channel_ = select_next_rmt_channel();
#endif
}
#ifdef ARDUINO_ARCH_ESP32
uint32_t RemoteControlComponentBase::from_microseconds(uint32_t us) {
  const uint32_t ticks_per_ten_us = 80000000u / this->clock_divider_ / 100000u;
  return us * ticks_per_ten_us / 10;
}
uint32_t RemoteControlComponentBase::to_microseconds(uint32_t ticks) {
  const uint32_t ticks_per_ten_us = 80000000u / this->clock_divider_ / 100000u;
  return (ticks * 10) / ticks_per_ten_us;
}
void RemoteControlComponentBase::set_channel(rmt_channel_t channel) {
  this->channel_ = channel;
}
void RemoteControlComponentBase::set_clock_divider(uint8_t clock_divider) {
  this->clock_divider_ = clock_divider;
}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
