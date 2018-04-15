//
//  hdc1080_component.h
//  esphomelib
//
//  Created by Otto Winter on 01.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_HDC1080_COMPONENT_H
#define ESPHOMELIB_SENSOR_HDC1080_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_HDC1080_SENSOR

namespace esphomelib {

namespace sensor {

class HDC1080TemperatureSensor;
class HDC1080HumiditySensor;

/** HDC1080 temperature+humidity i2c sensor integration.
 *
 * Based off of implementation by ClosedCube: https://github.com/closedcube/ClosedCube_HDC1080_Arduino
 */
class HDC1080Component : public PollingComponent {
 public:
  /// Initialize the component with the provided update interval.
  explicit HDC1080Component(const std::string &temperature_name, const std::string &humidity_name,
                            uint32_t update_interval);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup the sensor and check for connection.
  void setup() override;
  /// Retrieve the latest sensor values. This operation takes approximately 16ms.
  void update() override;

  /// Helper to send the provided command and read back the 16-bit unsigned value.
  uint16_t read_data_(uint8_t cmd);

  /// Get the internal temperature sensor.
  HDC1080TemperatureSensor *get_temperature_sensor() const;
  /// Get the internal humidity sensor.
  HDC1080HumiditySensor *get_humidity_sensor() const;

 protected:
  HDC1080TemperatureSensor *temperature_;
  HDC1080HumiditySensor *humidity_;
};

/// Helper class to expose the temperature of the HDC1080 as a sensor.
class HDC1080TemperatureSensor : public Sensor {
 public:
  explicit HDC1080TemperatureSensor(const std::string &name, HDC1080Component *parent);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
  int8_t accuracy_decimals() override;
 protected:
  HDC1080Component *parent_;
};

/// Helper class to expose the humidity of the HDC1080 as a sensor.
class HDC1080HumiditySensor : public Sensor {
 public:
  explicit HDC1080HumiditySensor(const std::string &name, HDC1080Component *parent);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
  int8_t accuracy_decimals() override;
 protected:
  HDC1080Component *parent_;
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_HDC1080_SENSOR

#endif //ESPHOMELIB_SENSOR_HDC1080_COMPONENT_H
