#ifndef ESPHOMELIB_SENSOR_PZEM004T_H
#define ESPHOMELIB_SENSOR_PZEM004T_H

#include "esphomelib/defines.h"

#ifdef USE_PZM004T

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/uart_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using PZEM004TVoltageSensor = EmptySensor<1, ICON_FLASH, UNIT_V>;
using PZEM004TCurrentSensor = EmptySensor<1, ICON_FLASH, UNIT_A>;
using PZEM004TPowerSensor = EmptySensor<1, ICON_FLASH, UNIT_W>;

class PZEM004TAddress {
 public:
  PZEM004TAddress(uint8_t address0, uint8_t address1, uint8_t address2, uint8_t address3);

  uint8_t operator[] (int i) const;

  void print_to(char *buffer);

 protected:
  uint8_t address_[4];
};

class PZEM004TBus;

class PZEM004T {
 public:
  void send_voltage(float voltage) {

  }
  void send_current(float current) {

  }
  void send_power(float power) {

  }
 protected:
  friend PZEM004TBus;

  PZEM004TAddress address_;
};

class PZEM004TBus : public PollingComponent, public UARTDevice {
 public:
  void setup() override;
  float get_setup_priority() const override;
  void update() override;
  void dump_config() override;
 protected:
  enum class PZEM004TActiveMeasurement {
    VOLTAGE = 0xB0,
    CURRENT = 0xB1,
    POWER = 0xB2,
    ENERGY = 0xB3,
    SET_ADDRESS = 0xB4,
  } active_measurement_{PZEM004TActiveMeasurement::VOLTAGE};

  void send_(const PZEM004TAddress &addr, PZEM004TActiveMeasurement command, uint8_t data);

  void receive_();

  void parse_rx_();

  uint8_t rx_buffer_[7];
  uint8_t rx_buffer_at_{0};
  std::vector<PZEM004T> pzems_;
  int active_index_{-1};
  bool setup_phase_{true};
  bool got_data_{false};
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_PZM004T

#endif //ESPHOMELIB_SENSOR_PZEM004T_H
