#ifndef ESPHOME_SENSOR_DHT_COMPONENT_H
#define ESPHOME_SENSOR_DHT_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_DHT_SENSOR

#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using DHTTemperatureSensor = EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using DHTHumiditySensor = EmptyPollingParentSensor<0, ICON_WATER_PERCENT, UNIT_PERCENT>;

enum DHTModel {
  DHT_MODEL_AUTO_DETECT = 0,
  DHT_MODEL_DHT11,
  DHT_MODEL_DHT22,
  DHT_MODEL_AM2302,
  DHT_MODEL_RHT03,
  DHT_MODEL_SI7021
};

/// Component for reading temperature/humidity measurements from DHT11/DHT22 sensors.
class DHTComponent : public PollingComponent {
 public:
  /** Construct a DHTComponent.
   *
   * @param pin The pin which DHT sensor is connected to.
   * @param update_interval The interval in ms the sensor should be checked.
   */
  DHTComponent(const std::string &temperature_name, const std::string &humidity_name, GPIOPin *pin,
               uint32_t update_interval = 60000);

  /** Manually select the DHT model.
   *
   * Valid values are:
   *
   *  - DHT_MODEL_AUTO_DETECT (default)
   *  - DHT_MODEL_DHT11
   *  - DHT_MODEL_DHT22
   *  - DHT_MODEL_AM2302
   *  - DHT_MODEL_RHT03
   *  - DHT_MODEL_SI7021
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
  void dump_config() override;
  /// Update sensor values and push them to the frontend.
  void update() override;
  /// HARDWARE_LATE setup priority.
  float get_setup_priority() const override;

 protected:
  bool read_sensor_(float *temperature, float *humidity, bool report_errors);

  GPIOPin *pin_;
  DHTModel model_{DHT_MODEL_AUTO_DETECT};
  bool is_auto_detect_{false};
  DHTTemperatureSensor *temperature_sensor_;
  DHTHumiditySensor *humidity_sensor_;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_DHT_SENSOR

#endif  // ESPHOME_SENSOR_DHT_COMPONENT_H
