//
//  htu21d_component.h
//  esphomelib
//
//  Created by Otto Winter on 27.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_HTU21D_COMPONENT_H
#define ESPHOMELIB_SENSOR_HTU21D_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_HTU21D_SENSOR

#include "HTU21D.h"

namespace esphomelib {

namespace sensor {

class HTU21DTemperatureSensor;
class HTU21DHumiditySensor;

/** This component represents the HTU21D i2c temperature+humidity sensor in esphomelib.
 *
 * It's basically an i2c-based accurate temperature and humidity sensor.
 * See https://www.adafruit.com/product/1899 for more information.
 */
class HTU21DComponent : public PollingComponent {
 public:
  /// Construct the HTU21D with the given update interval.
  explicit HTU21DComponent(uint32_t update_interval = 15000);

  /// Get a reference to the internal i2cdevlib HTU21D object used for communication.
  HTU21D &get_htu21d();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get a pointer to the temperature sensor object used to expose temperatures as a sensor.
  HTU21DTemperatureSensor *get_temperature_sensor() const;
  /// Get a pointer to the humidity sensor object used to expose humidities as a sensor.
  HTU21DHumiditySensor *get_humidity_sensor() const;

  /// Setup (reset) the sensor and check connection.
  void setup() override;
  /// Update the sensor values (temperature+humidity).
  void update() override;

 protected:
  HTU21D htu21d_;
  HTU21DTemperatureSensor *temperature_{nullptr};
  HTU21DHumiditySensor *humidity_{nullptr};
};

/// Internal helper class to expose temperatures from the HTU21D as a sensor.
class HTU21DTemperatureSensor : public Sensor {
 public:
  explicit HTU21DTemperatureSensor(HTU21DComponent *parent);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
  int8_t accuracy_decimals() override;

 protected:
  HTU21DComponent *parent_{nullptr};
};

/// Internal helper class to expose humidities from the HTU21D as a sensor.
class HTU21DHumiditySensor : public Sensor {
 public:
  explicit HTU21DHumiditySensor(HTU21DComponent *parent);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
  int8_t accuracy_decimals() override;

 protected:
  HTU21DComponent *parent_{nullptr};
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_HTU21D_SENSOR

#endif //ESPHOMELIB_SENSOR_HTU21D_COMPONENT_H
