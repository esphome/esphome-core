//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_INPUT_DHT_COMPONENT_H
#define ESPHOMELIB_INPUT_DHT_COMPONENT_H

#include "esphomelib/sensor/sensor.h"
#include <DHT.h>

namespace esphomelib {

namespace input {

/// DHTComponent - Component for reading temperature/humidity measurements from DHT11/DHT22 sensors.
class DHTComponent : public Component {
 public:
  /** Construct a DHTComponent.
   *
   * @param pin The pin which DHT sensor is connected to.
   * @param check_interval The interval in ms the sensor should be checked.
   */
  explicit DHTComponent(uint8_t pin, uint32_t check_interval = 1000);

  sensor::TemperatureSensor *get_temperature_sensor() const;
  sensor::HumiditySensor *get_humidity_sensor() const;

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);

  uint32_t get_check_interval() const;
  void set_check_interval(uint32_t check_interval);

  /** Manually select the DHT model.
   *
   * Valid values are: DHT::AUTO_DETECT (default), DHT::DHT11, DHT::DHT22, DHT::AM2302, and DHT::RHT03.
   *
   * @param model The DHT model.
   */
  void set_dht_model(DHT::DHT_MODEL_t model);

  const DHT &get_dht() const;

  void setup() override;
  float get_setup_priority() const override;

 protected:
  uint8_t pin_;
  uint32_t check_interval_;
  DHT dht_;
  DHT::DHT_MODEL_t model_{DHT::AUTO_DETECT};
  sensor::TemperatureSensor *temperature_sensor_;
  sensor::HumiditySensor *humidity_sensor_;
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_DHT_COMPONENT_H
