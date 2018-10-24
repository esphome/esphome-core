#ifndef ESPHOMELIB_SONOFF_RF_BRIDGE_H
#define ESPHOMELIB_SONOFF_RF_BRIDGE_H

#include "esphomelib/defines.h"

#ifdef USE_SONOFF_RF_BRIDGE

#include "esphomelib/component.h"
#include "esphomelib/uart_component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/switch_/switch.h"

ESPHOMELIB_NAMESPACE_BEGIN

class SonoffRFBinarySensor;
class SonoffRFSwitch;

class SonoffRFBridge : public UARTDevice, public Component {
 public:
  SonoffRFBridge(UARTComponent *parent);
  void setup() override;
  void loop() override;
  float get_setup_priority() const override;

  void send_code(uint16_t sync, uint16_t low, uint16_t high, uint32_t data);
  void send_code(uint16_t sync, uint16_t low, uint16_t high, const char *data);

  SonoffRFBinarySensor *make_binary_sensor(const std::string &name, uint16_t sync, uint16_t low, uint16_t high, uint32_t data);
  SonoffRFSwitch *make_switch(const std::string &name, uint16_t sync, uint16_t low, uint16_t high, uint32_t data);

 protected:
  bool check_byte_(uint8_t byte, uint8_t i);
  void process_data_();

  void ack_();

  void send_(uint8_t *message);

 uint8_t data_at_{0};
 uint8_t data_[16];
 uint32_t last_transmission_{0};
 std::vector<SonoffRFBinarySensor *> binary_sensors_;
};

class SonoffRFBinarySensor : public binary_sensor::BinarySensor {
 public:
  SonoffRFBinarySensor(const std::string &name, uint16_t sync, uint16_t low, uint16_t high, uint32_t data);

  bool process(uint16_t sync, uint16_t low, uint16_t high, uint32_t data);

 protected:
  uint16_t sync_;
  uint16_t low_;
  uint16_t high_;
  uint32_t data_;
};

class SonoffRFSwitch : public switch_::Switch {
 public:
  SonoffRFSwitch(const std::string &name, SonoffRFBridge *parent,
                 uint16_t sync, uint16_t low, uint16_t high, uint32_t data);

 protected:
  void write_state(bool state) override;

  SonoffRFBridge *parent_;
  uint16_t sync_;
  uint16_t low_;
  uint16_t high_;
  uint32_t data_;
};

ESPHOMELIB_NAMESPACE_END

#endif //USE_SONOFF_RF_BRIDGE

#endif //ESPHOMELIB_SONOFF_RF_BRIDGE_H
