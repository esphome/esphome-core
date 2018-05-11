//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_SENSOR_DHT_COMPONENT_H
#define ESPHOMELIB_SENSOR_DHT_COMPONENT_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_DHT_SENSOR

namespace esphomelib {

namespace sensor {

using DHTTemperatureSensor = EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using DHTHumiditySensor = EmptyPollingParentSensor<0, ICON_WATER_PERCENT, UNIT_PERCENT>;

enum class DHTModel {
  AUTO_DETECT = 0,
  DHT11,
  DHT22,
  AM2302,
  RHT03,
};

/// DHTComponent - Component for reading temperature/humidity measurements from DHT11/DHT22 sensors.
class DHTComponent : public PollingComponent {
 public:
  /** Construct a DHTComponent.
   *
   * @param pin The pin which DHT sensor is connected to.
   * @param update_interval The interval in ms the sensor should be checked.
   */
  DHTComponent(const std::string &temperature_name, const std::string &humidity_name,
               GPIOPin *pin, uint32_t update_interval = 15000);

  /** Manually select the DHT model.
   *
   * Valid values are: DHTModel::AUTO_DETECT (default), DHTModel::DHT11, DHTModel::DHT22,
   * DHTModel::AM2302 and DHTModel::RHT03.
   *
   * @param model The DHT model.
   */
  void set_dht_model(DHTModel model);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  DHTTemperatureSensor *get_temperature_sensor() const;
  DHTHumiditySensor *get_humidity_sensor() const;

  /// Set up the pins and check connection.
  void setup() override;
  /// Update sensor values and push them to the frontend.
  void update() override;
  /// HARDWARE_LATE setup priority.
  float get_setup_priority() const override;

 protected:
  uint8_t read_sensor_(float *temperature, float *humidity);
  uint8_t read_sensor_safe_(float *temperature, float *humidity);

  GPIOPin *pin_;
  DHTModel model_{DHTModel::AUTO_DETECT};
  DHTTemperatureSensor *temperature_sensor_;
  DHTHumiditySensor *humidity_sensor_;
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_DHT_SENSOR

#endif //ESPHOMELIB_SENSOR_DHT_COMPONENT_H
