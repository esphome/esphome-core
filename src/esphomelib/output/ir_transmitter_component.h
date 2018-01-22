//
// Created by Otto Winter on 01.12.17.
//

#ifndef ESPHOMELIB_OUTPUT_IR_TRANSMITTER_COMPONENT_H
#define ESPHOMELIB_OUTPUT_IR_TRANSMITTER_COMPONENT_H

#ifdef ARDUINO_ARCH_ESP32

#include <driver/rmt.h>
#include <esphomelib/component.h>
#include <esphomelib/switch_platform/switch.h>

namespace esphomelib {

namespace output {

namespace ir {

namespace nec {

const uint32_t CARRIER_FREQUENCY_HZ = 38000;
const uint32_t HEADER_HIGH_US = 9000;
const uint32_t HEADER_LOW_US = 4500;
const uint32_t BIT_HIGH_US = 560;
const uint32_t BIT_ONE_LOW_US = 1690;
const uint32_t BIT_ZERO_LOW_US = 560;

} // namespace nec

namespace lg {

const uint32_t CARRIER_FREQUENCY_HZ = 38000;
const uint32_t HEADER_HIGH_US = 8000;
const uint32_t HEADER_LOW_US = 4000;
const uint32_t BIT_HIGH_US = 600;
const uint32_t BIT_ONE_LOW_US = 1600;
const uint32_t BIT_ZERO_LOW_US = 550;

} // namespace lg

namespace sony {

const uint32_t CARRIER_FREQUENCY_HZ = 40000;
const uint32_t HEADER_HIGH_US = 2400;
const uint32_t HEADER_LOW_US = 600;
const uint32_t BIT_ONE_HIGH_US = 1200;
const uint32_t BIT_ZERO_HIGH_US = 600;
const uint32_t BIT_LOW_US = 600;

} // namespace sony

namespace panasonic {

const uint32_t CARRIER_FREQUENCY_HZ = 35000;
const uint32_t HEADER_HIGH_US = 3502;
const uint32_t HEADER_LOW_US = 1750;
const uint32_t BIT_HIGH_US = 502;
const uint32_t BIT_ZERO_LOW_US = 400;
const uint32_t BIT_ONE_LOW_US = 1244;

} // namespace panasonic

struct SendData {
  uint32_t carrier_frequency;
  std::vector<int16_t> data;
  uint16_t repeat_times{1};
  uint16_t repeat_wait;

  std::vector<rmt_item32_t> get_rmt_data(uint16_t ticks_for_10_us);

  void mark(uint16_t duration_us);
  void space(uint16_t duration_us);
  void add_item(uint16_t high_us, uint16_t low_us);

  SendData repeat(uint16_t times, uint16_t wait_us = 30000);

  uint32_t total_length_ms() const;

  static SendData from_nec(uint16_t address, uint16_t command);
  static SendData from_lg(uint32_t data, uint8_t nbits = 28);
  static SendData from_sony(uint32_t data, uint8_t nbits = 12);
  static SendData from_panasonic(uint16_t address, uint32_t data);
  static SendData from_raw(std::vector<int> &raw_data, uint32_t carrier_frequency);
};

} // namespace ir

const uint32_t BASE_CLOCK_HZ = 80000000;
const uint8_t DEFAULT_CLOCK_DIVIDER = 10;
const uint32_t DEFAULT_CARRIER_FREQUENCY_HZ = 38000;

class IRTransmitterComponent : public Component {
 public:
  explicit IRTransmitterComponent(uint8_t pin,
                                  uint8_t carrier_duty_percent = 50,
                                  uint8_t clock_divider = DEFAULT_CLOCK_DIVIDER);

  class DataTransmitter : public switch_platform::Switch {
   public:
    DataTransmitter(const ir::SendData &send_data, IRTransmitterComponent *parent);

    void write_state(bool state) override;
   private:
    ir::SendData send_data_;
    IRTransmitterComponent *parent_;
  };

  void setup() override;
  float get_setup_priority() const override;

  DataTransmitter *create_transmitter(const ir::SendData &send_data);
  void send(ir::SendData &send_data);

  rmt_channel_t get_channel() const;
  void set_channel(rmt_channel_t channel);
  uint8_t get_pin() const;
  void set_pin(uint8_t pin);
  uint8_t get_clock_divider() const;
  void set_clock_divider(uint8_t clock_divider);
  uint8_t get_carrier_duty_percent() const;
  void set_carrier_duty_percent(uint8_t carrier_duty_percent);

 protected:
  void require_carrier_frequency(uint32_t carrier_frequency);
  uint16_t get_ticks_for_10_us();
  void configure_rmt();

  rmt_channel_t channel_;
  uint8_t pin_;
  uint8_t clock_divider_;
  uint32_t last_carrier_frequency_;
  uint8_t carrier_duty_percent_;
};

extern rmt_channel_t next_rmt_channel;

} // namespace output

} // namespace esphomelib

#endif

#endif //ESPHOMELIB_OUTPUT_IR_TRANSMITTER_COMPONENT_H
