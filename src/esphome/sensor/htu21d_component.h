#ifndef ESPHOME_SENSOR_HTU21D_COMPONENT_H
#define ESPHOME_SENSOR_HTU21D_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_HTU21D_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using HTU21DTemperatureSensor = EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using HTU21DHumiditySensor = EmptyPollingParentSensor<0, ICON_WATER_PERCENT, UNIT_PERCENT>;

class HTU21DComponent : public PollingComponent, public I2CDevice {
 public:
  /// Construct the HTU21D with the given update interval.
  HTU21DComponent(I2CComponent *parent, const std::string &temperature_name, const std::string &humidity_name,
                  uint32_t update_interval = 60000);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get a pointer to the temperature sensor object used to expose temperatures as a sensor.
  HTU21DTemperatureSensor *get_temperature_sensor() const;
  /// Get a pointer to the humidity sensor object used to expose humidities as a sensor.
  HTU21DHumiditySensor *get_humidity_sensor() const;

  /// Setup (reset) the sensor and check connection.
  void setup() override;
  void dump_config() override;
  /// Update the sensor values (temperature+humidity).
  void update() override;

  float get_setup_priority() const override;

 protected:
  HTU21DTemperatureSensor *temperature_{nullptr};
  HTU21DHumiditySensor *humidity_{nullptr};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HTU21D_SENSOR

#endif  // ESPHOME_SENSOR_HTU21D_COMPONENT_H
