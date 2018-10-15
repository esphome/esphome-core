#ifndef ESPHOMELIB_REMOTE_REMOTE_PROTOCOL_H
#define ESPHOMELIB_REMOTE_REMOTE_PROTOCOL_H

#include "esphomelib/defines.h"

#ifdef USE_REMOTE

#include <vector>
#include "esphomelib/component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/switch_/switch.h"
#include "esphomelib/binary_sensor/binary_sensor.h"

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

  void set_carrier_frequency(uint32_t carrier_frequency);

  uint32_t get_carrier_frequency() const;

  const std::vector<int32_t> &get_data() const;

  void set_data(std::vector<int32_t> data);

  void reset();

  std::vector<int32_t>::iterator begin();

  std::vector<int32_t>::iterator end();

 protected:
  std::vector<int32_t> data_{};
  uint32_t carrier_frequency_;
};

class RemoteTransmitterComponent;

class RemoteTransmitter : public switch_::Switch {
 public:
  explicit RemoteTransmitter(const std::string &name);

  virtual void to_data(RemoteTransmitData *data) = 0;

  void set_parent(RemoteTransmitterComponent *parent);

  void set_repeat(uint32_t send_times, uint32_t send_wait);

 protected:
  void turn_on() override;
  void turn_off() override;

  RemoteTransmitterComponent *parent_;
  uint32_t send_times_{1}; ///< How many times to send the data
  uint32_t send_wait_{0}; ///< How many microseconds to wait between repeats.
};

class RemoteTransmitterComponent : public RemoteControlComponentBase, public Component {
 public:
  explicit RemoteTransmitterComponent(GPIOPin *pin);

  RemoteTransmitter *add_transmitter(RemoteTransmitter *transmitter);

  void send(RemoteTransmitData *data, uint32_t send_times, uint32_t send_wait);

  void setup() override;

  float get_setup_priority() const override;

  void set_carrier_duty_percent(uint8_t carrier_duty_percent);

 protected:
  friend RemoteTransmitter;

#ifdef ARDUINO_ARCH_ESP8266
  void calculate_on_off_time_(uint32_t carrier_frequency,
                              uint32_t *on_time_period,
                              uint32_t *off_time_period);

  void mark_(uint32_t on_time, uint32_t off_time, uint32_t usec);

  void space_(uint32_t usec);
#endif

#ifdef ARDUINO_ARCH_ESP32
  void configure_rmt();
  uint32_t current_carrier_frequency_{UINT32_MAX};
  bool initialized_{false};
  std::vector<rmt_item32_t> rmt_temp_;
#endif
  uint8_t carrier_duty_percent_{50};
  std::vector<RemoteTransmitter *> transmitters_{};
  RemoteTransmitData temp_;
};

#endif //USE_REMOTE_TRANSMITTER

#ifdef USE_REMOTE_RECEIVER

class RemoteReceiverComponent;

class RemoteReceiveData {
 public:
  RemoteReceiveData(RemoteReceiverComponent *parent, std::vector<int32_t> *data);

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

  int32_t pos(uint32_t index) const;

  int32_t operator [](uint32_t index) const;

  int32_t size() const;

 protected:
  uint32_t lower_bound_(uint32_t length);
  uint32_t upper_bound_(uint32_t length);

  RemoteReceiverComponent *parent_;
  uint32_t index_{0};
  std::vector<int32_t> *data_;
};

class RemoteReceiver : public binary_sensor::BinarySensor {
 public:
  explicit RemoteReceiver(const std::string &name);

  bool process_(RemoteReceiveData *data);

 protected:
  virtual bool matches(RemoteReceiveData *data) = 0;
};

class RemoteReceiveDumper {
 public:
  virtual void dump(RemoteReceiveData *data) = 0;

  void process_(RemoteReceiveData *data);
};

class RemoteReceiverComponent : public RemoteControlComponentBase, public Component {
 public:
  explicit RemoteReceiverComponent(GPIOPin *pin);

  void setup() override;
  void loop() override;
  float get_setup_priority() const override;

  RemoteReceiver *add_decoder(RemoteReceiver *decoder);
  void add_dumper(RemoteReceiveDumper *dumper);

  void set_buffer_size(uint32_t buffer_size);
  void set_tolerance(uint8_t tolerance);
  void set_filter_us(uint8_t filter_us);
  void set_idle_us(uint32_t idle_us);

 protected:
  friend RemoteReceiveData;

#ifdef ARDUINO_ARCH_ESP32
  void decode_rmt_(rmt_item32_t *item, size_t len);
#endif

#ifdef ARDUINO_ARCH_ESP32
  RingbufHandle_t ringbuf_;
#endif
#ifdef ARDUINO_ARCH_ESP8266
  /// Stores the time (in micros) that the leading/falling edge happened at
  ///  * An even index means a falling edge appeared at the time stored at the index
  ///  * An uneven index means a rising edge appeared at the time stored at the index
  volatile uint32_t *buffer_{nullptr};
  /// The position last written to
  volatile uint32_t buffer_write_at_;
  /// The position last read from
  uint32_t buffer_read_at_{0};
  void gpio_intr();
#endif

#ifdef ARDUINO_ARCH_ESP32
  uint32_t buffer_size_{10000};
#endif
  // On ESP8266, we can
#ifdef ARDUINO_ARCH_ESP8266
  uint32_t buffer_size_{1000};
#endif
  uint8_t tolerance_{25};
  std::vector<RemoteReceiver *> decoders_{};
  std::vector<RemoteReceiveDumper *> dumpers_{};
  uint8_t filter_us_{10};
  uint32_t idle_us_{10000};
  std::vector<int32_t> temp_;
};

#endif //USE_REMOTE_RECEIVER

} // namespace remote

ESPHOMELIB_NAMESPACE_END

#endif //USE_REMOTE

#endif //ESPHOMELIB_REMOTE_REMOTE_PROTOCOL_H
