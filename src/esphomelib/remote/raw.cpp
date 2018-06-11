//
//  raw.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/remote/raw.h"
#include "esphomelib/log.h"
#include <cstdio>
#include <utility>
#ifdef USE_REMOTE

ESPHOMELIB_NAMESPACE_BEGIN

namespace remote {

static const char *TAG = "remote.raw";

#ifdef USE_REMOTE_TRANSMITTER
RemoteTransmitData RawTransmitter::get_data() {
  RemoteTransmitData data;
  data.set_data(this->data_);
  data.set_carrier_frequency(this->carrier_frequency_);
  return data;
}
RawTransmitter::RawTransmitter(const std::string &name,
                               std::vector<int32_t> data,
                               uint32_t carrier_frequency)
    : RemoteTransmitter(name), data_(std::move(data)), carrier_frequency_(carrier_frequency) {

}
#endif

#ifdef USE_REMOTE_RECEIVER
void remote::RawDumper::dump(RemoteReceiveData &data) {
  char buffer[256];
  uint32_t buffer_offset = 0;
  buffer_offset += sprintf(buffer, "Received Raw: ");

  for (uint32_t i = 0; i < data.size(); i++) {
    const int32_t value = data.get_data()[i];
    const uint32_t remaining_length = sizeof(buffer) - buffer_offset;
    int written;

    if (i + 1 < data.size()) {
      written = snprintf(buffer + buffer_offset, remaining_length, "%d, ", value);
    } else {
      written = snprintf(buffer + buffer_offset, remaining_length, "%d", value);
    }

    if (written < 0 || written >= remaining_length) {
      // write failed, flush...
      buffer[buffer_offset] = '\0';
      ESP_LOGD(TAG, "%s", buffer);
      buffer_offset = 0;
      written = sprintf(buffer, "  ");
      if (i + 1 < data.size()) {
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
}
bool RawDecoder::matches(RemoteReceiveData &data) {
  for (int32_t val : this->data_) {
    if (val < 0) {
      if (!data.expect_space(-val))
        return false;
    } else {
      if (!data.expect_mark(val))
        return false;
    }
  }
}

RawDecoder::RawDecoder(const std::string &name, std::vector<int32_t> data)
    : RemoteReceiveDecoder(name), data_(std::move(data)) {}
#endif

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE
