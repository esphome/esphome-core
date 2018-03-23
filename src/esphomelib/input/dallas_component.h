//
// Created by Otto Winter on 28.12.17.
//

#ifndef ESPHOMELIB_INPUT_DALLAS_COMPONENT_H
#define ESPHOMELIB_INPUT_DALLAS_COMPONENT_H

#include <DallasTemperature.h>

#include "esphomelib/sensor/sensor.h"

namespace esphomelib {

namespace input {

class DallasTemperatureSensor : public sensor::TemperatureSensor, public PollingObject {
 public:
  DallasTemperatureSensor(uint64_t address, uint8_t resolution, uint32_t update_interval);

  uint8_t *get_address8();
  std::string get_name();

  uint64_t get_address() const;
  void set_address(uint64_t address);
  uint8_t get_index() const;
  void set_index(uint8_t index);
  uint8_t get_resolution() const;
  void set_resolution(uint8_t resolution);
  uint16_t millis_to_wait_for_conversion() const;

 protected:
  uint64_t address_;
  uint8_t index_;
  uint8_t resolution_;
  std::string name_;
};

/// DallasComponent - Component for reading temperature measurements from Dallas sensors.
class DallasComponent : public Component {
 public:

  explicit DallasComponent(OneWire *one_wire);

  DallasTemperatureSensor *get_sensor_by_address(uint64_t address,
                                                 uint32_t update_interval = 10000,
                                                 uint8_t resolution = 12);
  DallasTemperatureSensor *get_sensor_by_index(uint8_t index,
                                               uint32_t update_interval = 10000,
                                               uint8_t resolution = 12);
  uint8_t get_device_count();

  DallasTemperature &get_dallas();
  OneWire *get_one_wire() const;
  void set_one_wire(OneWire *one_wire);

  std::vector<uint64_t> scan_devices();

  void setup() override;
  float get_setup_priority() const override;

 protected:
  void request_temperature(DallasTemperatureSensor *sensor);

  DallasTemperature dallas_;
  OneWire *one_wire_;
  std::vector<DallasTemperatureSensor *> sensors_;
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_DALLAS_COMPONENT_H