#ifndef ESPHOME_SENSOR_DALLAS_COMPONENT_H
#define ESPHOME_SENSOR_DALLAS_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_DALLAS_SENSOR

#include "esphome/sensor/sensor.h"
#include "esphome/esp_one_wire.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class DallasTemperatureSensor;

/** Hub for dealing with dallas temperature sensor. Uses a OneWire interface.
 *
 * Get the individual sensors with `get_sensor_by_address` or `get_sensor_by_index`.
 */
class DallasComponent : public PollingComponent {
 public:
  /// Construct the DallasComponent hub with the given OneWire instance pointer.
  explicit DallasComponent(ESPOneWire *one_wire, uint32_t update_interval);

  /** Get a DallasTemperatureSensor by address.
   *
   * @param address 64-bit unsigned address for this sensor. Check debug logs for getting this.
   * @param resolution The resolution for this sensor, 8-12.
   * @return A pointer to a DallasTemperatureSensor, use this to setup MQTT.
   */
  DallasTemperatureSensor *get_sensor_by_address(const std::string &name, uint64_t address, uint8_t resolution = 12);
  /** Get a DallasTemperatureSensor by index.
   *
   * Note: It is recommended to use sensors by address to avoid mixing up sensor values
   * if one sensor can't be found (and therefore receives an incorrect index).
   *
   * @param index The index of this sensor, starts with 0.
   * @param resolution The resolution for this sensor, 8-12.
   * @return A pointer to a DallasTemperatureSensor, use this to setup MQTT.
   */
  DallasTemperatureSensor *get_sensor_by_index(const std::string &name, uint8_t index, uint8_t resolution = 12);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get the ESPOneWire instance used for this hub.
  /// Manually set the ESPOneWire instance used for this hub.
  void set_one_wire(ESPOneWire *one_wire);

  /// Set up individual sensors and update intervals.
  void setup() override;
  void dump_config() override;
  /// HARDWARE_LATE setup priority.
  float get_setup_priority() const override;

  void update() override;

  ESPOneWire *get_one_wire() const;

 protected:
  ESPOneWire *one_wire_;
  std::vector<DallasTemperatureSensor *> sensors_;
  std::vector<uint64_t> found_sensors_;
};

/// Internal class that helps us create multiple sensors for one Dallas hub.
class DallasTemperatureSensor : public EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C, DallasComponent> {
 public:
  /** Construct the temperature sensor with the given address.
   *
   * @param address 64-bit unsigned address of the temperature sensor. Can be 0 to indicate using index.
   * @param resolution Resolution used for this sensor. Usually 8-12.
   * @param update_interval The interval in ms the sensor should be checked.
   */
  DallasTemperatureSensor(const std::string &name, uint64_t address, uint8_t resolution, DallasComponent *parent);

  /// Helper to get a pointer to the address as uint8_t.
  uint8_t *get_address8();
  /// Helper to create (and cache) the name for this sensor. For example "0xfe0000031f1eaf29".
  const std::string &get_address_name();

  /// Get the 64-bit unsigned address for this sensor.
  uint64_t get_address() const;
  /// Set the 64-bit unsigned address for this sensor.
  void set_address(uint64_t address);
  /// Get the index of this sensor. (0 if using address.)
  optional<uint8_t> get_index() const;
  /// Set the index of this sensor. If using index, address will be set after setup.
  void set_index(uint8_t index);
  /// Get the set resolution for this sensor.
  uint8_t get_resolution() const;
  /// Set the resolution for this sensor.
  void set_resolution(uint8_t resolution);
  /// Get the number of milliseconds we have to wait for the conversion phase.
  uint16_t millis_to_wait_for_conversion() const;

  bool setup_sensor();
  bool read_scratch_pad();

  bool check_scratch_pad();

  float get_temp_c();

  std::string unique_id() override;

 protected:
  uint64_t address_;
  optional<uint8_t> index_;

  uint8_t resolution_;
  std::string address_name_;
  uint8_t scratch_pad_[9] = {
      0,
  };
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_DALLAS_SENSOR

#endif  // ESPHOME_SENSOR_DALLAS_COMPONENT_H
