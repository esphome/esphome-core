#include "esphome/defines.h"

#ifdef USE_REMOTE

#include "esphome/remote/raw.h"
#include "esphome/log.h"
#include <cstdio>
#include <utility>

ESPHOME_NAMESPACE_BEGIN

namespace remote {

#ifdef USE_REMOTE_RECEIVER
static const char *TAG = "remote.raw";
#endif

#ifdef USE_REMOTE_TRANSMITTER
void RawTransmitter::to_data(RemoteTransmitData *data) {
  data->reserve(this->len_);
  for (size_t i = 0; i < this->len_; i++) {
    auto val = this->data_[i];
    if (val < 0)
      data->space(static_cast<uint32_t>(-val));
    else
      data->mark(static_cast<uint32_t>(val));
  }
  data->set_carrier_frequency(this->carrier_frequency_);
}
RawTransmitter::RawTransmitter(const std::string &name, const int32_t *data, size_t len, uint32_t carrier_frequency)
    : RemoteTransmitter(name), data_(data), len_(len), carrier_frequency_(carrier_frequency) {}
#endif

#ifdef USE_REMOTE_RECEIVER
bool RawDumper::dump(RemoteReceiveData *data) {
  char buffer[256];
  uint32_t buffer_offset = 0;
  buffer_offset += sprintf(buffer, "Received Raw: ");

  for (int32_t i = 0; i < data->size(); i++) {
    const int32_t value = (*data)[i];
    const uint32_t remaining_length = sizeof(buffer) - buffer_offset;
    int written;

    if (i + 1 < data->size()) {
      written = snprintf(buffer + buffer_offset, remaining_length, "%d, ", value);
    } else {
      written = snprintf(buffer + buffer_offset, remaining_length, "%d", value);
    }

    if (written < 0 || written >= int(remaining_length)) {
      // write failed, flush...
      buffer[buffer_offset] = '\0';
      ESP_LOGD(TAG, "%s", buffer);
      buffer_offset = 0;
      written = sprintf(buffer, "  ");
      if (i + 1 < data->size()) {
        written += sprintf(buffer + written, "%d, ", value);
      } else {
        written += sprintf(buffer + written, "%d", value);
      }
    }

    buffer_offset += written;
  }
  if (buffer_offset != 0) {
    ESP_LOGD(TAG, "%s", buffer);
  }

  return true;
}
bool RawDumper::is_secondary() { return true; }
bool RawReceiver::matches(RemoteReceiveData *data) {
  for (size_t i = 0; i < this->len_; i++) {
    auto val = this->data_[i];
    if (val < 0) {
      if (!data->expect_space(static_cast<uint32_t>(-val)))
        return false;
    } else {
      if (!data->expect_mark(static_cast<uint32_t>(val)))
        return false;
    }
  }
  return true;
}

RawReceiver::RawReceiver(const std::string &name, const int32_t *data, size_t len)
    : RemoteReceiver(name), data_(data), len_(len) {}
#endif

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE
