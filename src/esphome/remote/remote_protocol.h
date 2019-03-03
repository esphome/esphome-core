#ifndef ESPHOME_REMOTE_REMOTE_PROTOCOL_H
#define ESPHOME_REMOTE_REMOTE_PROTOCOL_H

#include "esphome/defines.h"

#ifdef USE_REMOTE

#include <vector>
#include "esphome/component.h"
#include "esphome/esphal.h"

#ifdef ARDUINO_ARCH_ESP32
#include <driver/rmt.h>
#endif

ESPHOME_NAMESPACE_BEGIN

namespace remote {

class RemoteControlComponentBase {
 public:
  explicit RemoteControlComponentBase(GPIOPin *pin);

#ifdef ARDUINO_ARCH_ESP32
  void set_channel(rmt_channel_t channel);
  void set_clock_divider(uint8_t clock_divider);
#endif

 protected:
#ifdef ARDUINO_ARCH_ESP32
  uint32_t from_microseconds(uint32_t us);
  uint32_t to_microseconds(uint32_t ticks);
#endif

  GPIOPin *pin_;
#ifdef ARDUINO_ARCH_ESP32
  rmt_channel_t channel_{RMT_CHANNEL_0};
  uint8_t clock_divider_{80};
  esp_err_t error_code_{ESP_OK};
#endif
};

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE

#endif  // ESPHOME_REMOTE_REMOTE_PROTOCOL_H
