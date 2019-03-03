#ifndef ESPHOME_REMOTE_REMOTE_RECEIVER_H
#define ESPHOME_REMOTE_REMOTE_RECEIVER_H

#include "esphome/defines.h"

#ifdef USE_REMOTE_RECEIVER

#include "esphome/component.h"
#include "esphome/remote/remote_protocol.h"
#include "esphome/switch_/switch.h"
#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

class RemoteReceiverComponent;

struct JVCDecodeData {
  bool valid;
  uint32_t data;
};

struct LGDecodeData {
  bool valid;
  uint32_t data;
  uint8_t nbits;
};

struct NECDecodeData {
  bool valid;
  uint16_t address;
  uint16_t command;
};

struct PanasonicDecodeData {
  bool valid;
  uint16_t address;
  uint32_t command;
};

struct SamsungDecodeData {
  bool valid;
  uint32_t data;
};

struct SonyDecodeData {
  bool valid;
  uint32_t data;
  uint8_t nbits;
};

struct RC5DecodeData {
  bool valid;
  uint8_t address;
  uint8_t command;
};

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

  int32_t operator[](uint32_t index) const;

  int32_t size() const;

  JVCDecodeData decode_jvc();
  LGDecodeData decode_lg();
  NECDecodeData decode_nec();
  PanasonicDecodeData decode_panasonic();
  SamsungDecodeData decode_samsung();
  SonyDecodeData decode_sony();

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

  bool process(RemoteReceiveData *data);

 protected:
  virtual bool matches(RemoteReceiveData *data) = 0;
};

class RemoteReceiveDumper {
 public:
  virtual bool dump(RemoteReceiveData *data) = 0;

  bool process(RemoteReceiveData *data);

  virtual bool is_secondary();
};

struct RemoteReceiverComponentStore {
  static void gpio_intr(RemoteReceiverComponentStore *arg);

  /// Stores the time (in micros) that the leading/falling edge happened at
  ///  * An even index means a falling edge appeared at the time stored at the index
  ///  * An uneven index means a rising edge appeared at the time stored at the index
  volatile uint32_t *buffer{nullptr};
  /// The position last written to
  volatile uint32_t buffer_write_at;
  /// The position last read from
  uint32_t buffer_read_at{0};
  bool overflow{false};
  uint32_t buffer_size{1000};
  uint8_t filter_us{10};
  ISRInternalGPIOPin *pin;
};

class RemoteReceiverComponent : public RemoteControlComponentBase, public Component {
 public:
  explicit RemoteReceiverComponent(GPIOPin *pin);

  void setup() override;
  void dump_config() override;
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

  void process_(RemoteReceiveData *data);

#ifdef ARDUINO_ARCH_ESP32
  void decode_rmt_(rmt_item32_t *item, size_t len);
#endif

#ifdef ARDUINO_ARCH_ESP32
  RingbufHandle_t ringbuf_;
#endif
#ifdef ARDUINO_ARCH_ESP8266
  RemoteReceiverComponentStore store_;
#endif

#ifdef ARDUINO_ARCH_ESP32
  uint32_t buffer_size_{10000};
#endif
#ifdef ARDUINO_ARCH_ESP8266
  uint32_t buffer_size_{1000};
  HighFrequencyLoopRequester high_freq_;
#endif
  uint8_t tolerance_{25};
  std::vector<RemoteReceiver *> decoders_{};
  std::vector<RemoteReceiveDumper *> dumpers_{};
  uint8_t filter_us_{10};
  uint32_t idle_us_{10000};
  std::vector<int32_t> temp_;
};

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE_RECEIVER

#endif  // ESPHOME_REMOTE_REMOTE_RECEIVER_H
