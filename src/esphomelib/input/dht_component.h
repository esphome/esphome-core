//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_INPUT_DHT_COMPONENT_H
#define ESPHOMELIB_INPUT_DHT_COMPONENT_H

#include <DHT.h>

#include "esphomelib/sensor/sensor.h"

namespace esphomelib {

namespace input {

class DHTComponent;

class DHTTemperatureSensor : public sensor::Sensor {
 public:
  explicit DHTTemperatureSensor(DHTComponent *parent);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
 protected:
  DHTComponent *parent_{nullptr};
};

class DHTHumiditySensor : public sensor::Sensor {
 public:
  explicit DHTHumiditySensor(DHTComponent *parent);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
 protected:
  DHTComponent *parent_{nullptr};
};

/// DHTComponent - Component for reading temperature/humidity measurements from DHT11/DHT22 sensors.
class DHTComponent : public PollingComponent {
 public:
  /** Construct a DHTComponent.
   *
   * @param pin The pin which DHT sensor is connected to.
   * @param update_interval The interval in ms the sensor should be checked.
   */
  explicit DHTComponent(uint8_t pin, uint32_t update_interval = 15000);

  void set_pin(uint8_t pin);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  uint8_t get_pin() const;
  DHTTemperatureSensor *get_temperature_sensor() const;
  DHTHumiditySensor *get_humidity_sensor() const;

  /** Manually select the DHT model.
   *
   * Valid values are: DHT::AUTO_DETECT (default), DHT::DHT11, DHT::DHT22, DHT::AM2302, and DHT::RHT03.
   *
   * @param model The DHT model.
   */
  void set_dht_model(DHT::DHT_MODEL_t model);

  const DHT &get_dht() const;

  void setup() override;
  void update() override;
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
