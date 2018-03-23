//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_INPUT_DHT_COMPONENT_H
#define ESPHOMELIB_INPUT_DHT_COMPONENT_H

#include <DHT.h>

#include "esphomelib/sensor/sensor.h"

namespace esphomelib {

namespace input {

class DHTTemperatureSensor : public sensor::TemperatureSensor, public PollingObject {
 public:
  explicit DHTTemperatureSensor(uint32_t update_interval);
};

class DHTHumiditySensor : public sensor::HumiditySensor, public PollingObject {
 public:
  explicit DHTHumiditySensor(uint32_t update_interval);
};

/// DHTComponent - Component for reading temperature/humidity measurements from DHT11/DHT22 sensors.
class DHTComponent : public Component, public PollingObject {
 public:
  /** Construct a DHTComponent.
   *
   * @param pin The pin which DHT sensor is connected to.
   * @param update_interval The interval in ms the sensor should be checked.
   */
  explicit DHTComponent(uint8_t pin, uint32_t update_interval = 15000);

  void set_pin(uint8_t pin);
  void set_update_interval(uint32_t update_interval) override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  uint8_t get_pin() const;
  sensor::TemperatureSensor *get_temperature_sensor() const;
  sensor::HumiditySensor *get_humidity_sensor() const;

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
  DHT dht_;
  DHT::DHT_MODEL_t model_{DHT::AUTO_DETECT};
  DHTTemperatureSensor *temperature_sensor_;
  DHTHumiditySensor *humidity_sensor_;
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_DHT_COMPONENT_H
