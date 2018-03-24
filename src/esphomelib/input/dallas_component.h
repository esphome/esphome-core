//
// Created by Otto Winter on 28.12.17.
//

#ifndef ESPHOMELIB_INPUT_DALLAS_COMPONENT_H
#define ESPHOMELIB_INPUT_DALLAS_COMPONENT_H

#include <DallasTemperature.h>

#include "esphomelib/sensor/sensor.h"

namespace esphomelib {

namespace input {

class DallasTemperatureSensor;

/** Hub for dealing with dallas temperature sensor. Uses a OneWire interface.
 *
 * Get the individual sensors with `get_sensor_by_address` or `get_sensor_by_index`.
 */
class DallasComponent : public Component {
 public:
  /// Construct the DallasComponent hub with the given OneWire instance pointer.
  explicit DallasComponent(OneWire *one_wire);

  /** Get a DallasTemperatureSensor by address.
   *
   * @param address 64-bit unsigned address for this sensor. Check debug logs for getting this.
   * @param update_interval The interval in ms that the sensor should be checked.
   * @param resolution The resolution for this sensor, 8-12.
   * @return A pointer to a DallasTemperatureSensor, use this to setup MQTT.
   */
  DallasTemperatureSensor *get_sensor_by_address(uint64_t address,
                                                 uint32_t update_interval = 10000,
                                                 uint8_t resolution = 12);
  /** Get a DallasTemperatureSensor by index.
   *
   * Note: It is recommended to use sensors by address to avoid mixing up sensor values
   * if one sensor can't be found (and therefore receives an incorrect index).
   *
   * @param index The index of this sensor, starts with 0.
   * @param update_interval The interval in ms that the sensor should be checked.
   * @param resolution The resolution for this sensor, 8-12.
   * @return A pointer to a DallasTemperatureSensor, use this to setup MQTT.
   */
  DallasTemperatureSensor *get_sensor_by_index(uint8_t index,
                                               uint32_t update_interval = 10000,
                                               uint8_t resolution = 12);

  /// Scan all devices on the bus. You can also scroll through the debug logs to get this.
  std::vector<uint64_t> scan_devices();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get the internal dallas library instance.
  DallasTemperature &get_dallas();
  /// Get the OneWire instance used for this hub.
  OneWire *get_one_wire() const;
  /// Manually set the OneWire instance used for this hub.
  void set_one_wire(OneWire *one_wire);

  /// Set up individual sensors and update intervals.
  void setup() override;
  /// HARDWARE_LATE setup priority.
  float get_setup_priority() const override;

 protected:
  /// Request a temperature reading for the given DallasTemperatureSensor.
  void request_temperature(DallasTemperatureSensor *sensor);

  DallasTemperature dallas_;
  OneWire *one_wire_;
  std::vector<DallasTemperatureSensor *> sensors_;
};

/// Internal class that helps us create multiple sensors for one Dallas hub.
class DallasTemperatureSensor : public sensor::Sensor {
 public:
  /** Construct the temperature sensor with the given address.
   *
   * @param address 64-bit unsigned address of the temperature sensor. Can be 0 to indicate using index.
   * @param resolution Resolution used for this sensor. Usually 8-12.
   * @param update_interval The interval in ms the sensor should be checked.
   */
  DallasTemperatureSensor(uint64_t address, uint8_t resolution, uint32_t update_interval);

  /// Helper to get a pointer to the address as uint8_t.
  uint8_t *get_address8();
  /// Helper to create (and cache) the name for this sensor. For example "0xfe0000031f1eaf29".
  std::string get_name();

  /// Get the 64-bit unsigned address for this sensor.
  uint64_t get_address() const;
  /// Set the 64-bit unsigned address for this sensor.
  void set_address(uint64_t address);
  /// Get the index of this sensor. (0 if using address.)
  uint8_t get_index() const;
  /// Set the index of this sensor. If using index, address will be set after setup.
  void set_index(uint8_t index);
  /// Get the set resolution for this sensor.
  uint8_t get_resolution() const;
  /// Set the resolution for this sensor.
  void set_resolution(uint8_t resolution);
  /// Get the number of milliseconds we have to wait for the conversion phase.
  uint16_t millis_to_wait_for_conversion() const;
  /// Get the interval in ms that we will check the sensor for new values.
  uint32_t get_update_interval() const;
  /// Set the interval in ms that we will check the sensor for new values.
  void set_update_interval(uint32_t update_interval);

  /// Unit of measurement for MQTT: "°C".
  std::string unit_of_measurement() override;
  /// Icon for MQTT: ""
  std::string icon() override;
  /// Update interval for MQTT's automatic expiry: `update_interval`.
  uint32_t update_interval() override;
  /// Accuracy in decimals for this sensor. 1.
  int8_t accuracy_decimals() override;

 protected:
  uint64_t address_;
  uint8_t index_;
  uint8_t resolution_;
  std::string name_;
  uint32_t update_interval_;
};

} // namespace input

} // namespace esphomelib

#endif //ESPHOMELIB_INPUT_DALLAS_COMPONENT_H