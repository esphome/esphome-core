#ifndef ESPHOMELIB_SENSOR_HTU21D_COMPONENT_H
#define ESPHOMELIB_SENSOR_HTU21D_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_HTU21D_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/i2c_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using HTU21DTemperatureSensor = EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using HTU21DHumiditySensor = EmptyPollingParentSensor<0, ICON_WATER_PERCENT, UNIT_PERCENT>;

/** This component represents the HTU21D i2c temperature+humidity sensor in esphomelib.
 *
 * It's basically an i2c-based accurate temperature and humidity sensor.
 * See https://www.adafruit.com/product/1899 for more information.
 */
class HTU21DComponent : public PollingComponent, public I2CDevice {
 public:
  /// Construct the HTU21D with the given update interval.
  HTU21DComponent(I2CComponent *parent,
                  const std::string &temperature_name, const std::string &humidity_name,
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

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_HTU21D_SENSOR

#endif //ESPHOMELIB_SENSOR_HTU21D_COMPONENT_H
