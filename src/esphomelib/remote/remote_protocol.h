//
//  remote_protocol.h
//  esphomelib
//
//  Created by Otto Winter on 05.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_REMOTE_PROTOCOL_H
#define ESPHOMELIB_REMOTE_PROTOCOL_H

#include <vector>
#include <cstdint>
#include "esphomelib/component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/switch_/switch.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/defines.h"
#include "../../../../../../.platformio/packages/framework-arduinoespressif32@1.4.0/tools/sdk/include/freertos/freertos/ringbuf.h"

#ifdef USE_REMOTE

#ifdef ARDUINO_ARCH_ESP32
  #include <driver/rmt.h>
#endif

ESPHOMELIB_NAMESPACE_BEGIN

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
#endif
};

#ifdef USE_REMOTE_TRANSMITTER
class RemoteTransmitData {
 public:
  void mark(uint32_t length);

  void space(uint32_t length);

  void item(uint32_t mark, uint32_t space);

  void reserve(uint32_t len);

  void set_data(std::vector<int32_t> data);

  void set_carrier_frequency(uint32_t carrier_frequency);

  uint32_t get_carrier_frequency() const;

  const std::vector<int32_t> &get_data() const;

 protected:
  std::vector<int32_t> data_{};
  uint32_t carrier_frequency_;
};

class RemoteTransmitterComponent;

class RemoteTransmitter : public switch_::Switch {
 public:
  RemoteTransmitter(RemoteTransmitterComponent *parent, const std::string &name);

  virtual RemoteTransmitData get_data() = 0;

 protected:
  void turn_on() override;
  void turn_off() override;

  RemoteTransmitterComponent *parent_;
  uint32_t send_times_{1}; ///< How many times to send the data
  uint32_t send_wait_{0}; ///< How many milliseconds to wait between repeats.
};

class RemoteTransmitterComponent : public RemoteControlComponentBase, public Component {
 public:
  explicit RemoteTransmitterComponent(GPIOPin *pin);

  void send(const RemoteTransmitData &data, uint32_t send_times, uint32_t send_wait);

  void setup() override;

  float get_setup_priority() const override;

 protected:
  void calculate_on_off_time_(uint32_t carrier_frequency,
                              uint32_t *on_time_period,
                              uint32_t *off_time_period);

  void mark_(uint32_t on_time, uint32_t off_time, uint32_t usec);

  void space_(uint32_t usec);

#ifdef ARDUINO_ARCH_ESP32
  void configure_rmt();
#endif

#ifdef ARDUINO_ARCH_ESP32
  uint32_t current_carrier_frequency_;
#endif
  uint8_t carrier_duty_percent_{50};
};

#endif //USE_REMOTE_TRANSMITTER

#ifdef USE_REMOTE_RECEIVER

class RemoteReceiverComponent;

class RemoteReceiveData {
 public:
  RemoteReceiveData(RemoteReceiverComponent *parent, const std::vector<int32_t> &data);

  bool peek_mark(uint32_t length, uint32_t offset = 0);

  bool peek_space(uint32_t length, uint32_t offset = 0);

  bool peek_space_at_least(uint32_t length, uint32_t offset = 0);

  bool peek_item(uint32_t mark, uint32_t space, uint32_t offset = 0);

  int32_t peek(uint32_t offset = 0);

  void advance(uint32_t amount = 1);

  bool expect_mark(uint32_t length);
  bool expect_space(uint32_t length);
  bool expect_item(uint32_t mark, uint32_t space);

  void reset_index();

  const std::vector<int32_t> &get_data() const;

  uint32_t size() {
    return this->data_.size();
  }

 protected:
  uint32_t lower_bound_(uint32_t length);
  uint32_t upper_bound_(uint32_t length);

  RemoteReceiverComponent *parent_;
  uint32_t index_{0};
  std::vector<int32_t> data_;
};

class RemoteReceiveDecoder : public binary_sensor::BinarySensor {
 public:
  explicit RemoteReceiveDecoder(const std::string &name);

  bool process_(RemoteReceiveData &data);

 protected:
  virtual bool matches(RemoteReceiveData &data) = 0;
};

class RemoteReceiveDumper {
 public:
  virtual void dump(RemoteReceiveData &data) = 0;

  void process_(RemoteReceiveData &data);
};

class RemoteReceiverComponent : public RemoteControlComponentBase, public Component {
 public:
  explicit RemoteReceiverComponent(GPIOPin *pin);

  void setup() override;
  void loop() override;
  float get_setup_priority() const override;

  void add_decoder(RemoteReceiveDecoder *decoder);
  void add_dumper(RemoteReceiveDumper *dumper);

 protected:
  friend RemoteReceiveData;

#ifdef ARDUINO_ARCH_ESP32
  void configure_rmt_();
  std::vector<int32_t> decode_rmt_(rmt_item32_t *item, size_t len);
#endif

#ifdef ARDUINO_ARCH_ESP32
  RingbufHandle_t ringbuf_;
#endif

  uint32_t buffer_size_{10000};
  uint8_t tolerance_{25};
  std::vector<RemoteReceiveDecoder *> decoders_{};
  std::vector<RemoteReceiveDumper *> dumpers_{};
  uint8_t filter_us_{10};
  uint32_t idle_us_{10000};
};
#endif //USE_REMOTE_RECEIVER

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_PROTOCOL_H
