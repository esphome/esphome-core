#ifndef ESPHOME_REMOTE_TRANSMITTER_H
#define ESPHOME_REMOTE_TRANSMITTER_H

#include "esphome/defines.h"

#ifdef USE_REMOTE_TRANSMITTER

#include "esphome/component.h"
#include "esphome/remote/remote_protocol.h"
#include "esphome/remote/rc_switch_protocol.h"
#include "esphome/switch_/switch.h"

ESPHOME_NAMESPACE_BEGIN

namespace remote {

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
  uint32_t get_send_times() const;
  uint32_t get_send_wait() const;

 protected:
  void write_state(bool state) override;

  RemoteTransmitterComponent *parent_;
  uint32_t send_times_{1};  ///< How many times to send the data
  uint32_t send_wait_{0};   ///< How many microseconds to wait between repeats.
};

class RemoteTransmitterComponent : public RemoteControlComponentBase, public Component {
 public:
  explicit RemoteTransmitterComponent(GPIOPin *pin);

  RemoteTransmitter *add_transmitter(RemoteTransmitter *transmitter);

  void setup() override;

  void dump_config() override;

  float get_setup_priority() const override;

  void set_carrier_duty_percent(uint8_t carrier_duty_percent);

  /// Defer send of the switches remote code until next loop().
  void deferred_send(RemoteTransmitter *a_switch);

  class TransmitCall {
   public:
    TransmitCall(RemoteTransmitterComponent *parent);
    void set_jvc(uint32_t data);
    void set_lg(uint32_t data, uint8_t nbits);
    void set_nec(uint16_t address, uint16_t command);
    void set_panasonic(uint16_t address, uint32_t command);
    void set_raw(std::vector<int32_t> data);
    void set_rc5(uint8_t address, uint8_t command, bool toggle);
    void set_rc_switch_raw(uint32_t code, uint8_t nbits, RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_raw(const char *code, RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_raw_tristate(const char *code, RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_type_a(uint8_t switch_group, uint8_t switch_device, bool state,
                              RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_type_a(const char *switch_group, const char *switch_device, bool state,
                              RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_type_b(uint8_t address, uint8_t channel, bool state,
                              RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_type_c(uint8_t family, uint8_t group, uint8_t device, bool state,
                              RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_type_c(char family, uint8_t group, uint8_t device, bool state,
                              RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_type_d(uint8_t group, uint8_t device, bool state,
                              RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_rc_switch_type_d(char group, uint8_t device, bool state,
                              RCSwitchProtocol protocol = rc_switch_protocols[1]);
    void set_samsung(uint32_t data);
    void set_sony(uint32_t data, uint8_t nbits);

    void perform();

    RemoteTransmitData *get_data();

   protected:
    RemoteTransmitterComponent *parent_;
    uint32_t send_times_{1};
    uint32_t send_wait_{0};
  };

  TransmitCall transmit();

 protected:
  friend RemoteTransmitter;

  void send_(RemoteTransmitData *data, uint32_t send_times, uint32_t send_wait);

#ifdef ARDUINO_ARCH_ESP8266
  void calculate_on_off_time_(uint32_t carrier_frequency, uint32_t *on_time_period, uint32_t *off_time_period);

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

}  // namespace remote

ESPHOME_NAMESPACE_END

#endif  // USE_REMOTE_TRANSMITTER

#endif  // ESPHOME_REMOTE_TRANSMITTER_H
