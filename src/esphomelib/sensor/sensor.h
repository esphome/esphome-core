//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_SENSOR_SENSOR_H
#define ESPHOMELIB_SENSOR_SENSOR_H

#include <functional>

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"

namespace esphomelib {

namespace sensor {

using sensor_callback_t = std::function<void(float, int8_t)>;

/** Base-class for all sensors.
 *
 * A sensor has unit of measurement and can use push_new_value to send out a new value with the specified accuracy.
 */
class Sensor {
 public:
  // ========== OVERRIDE METHODS ==========
  // (You'll only need this when creating your own custom sensor)
  /** Push a new value to the MQTT front-end.
   *
   * Note that you should publish the raw value here, i.e. without any rounding as the user
   * can later override this accuracy.
   *
   * @param value The floating point value.
   * @param accuracy_decimals The accuracy in decimal points. The user can customize this.
   */
  void push_new_value(float value, int8_t accuracy_decimals);

  /** Override this to set the Home Assistant unit of measurement for this sensor.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this sensor, for example "°C".
   */
  virtual std::string unit_of_measurement();

  /** Override this to set the Home Assistant icon for this sensor.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this sensor, for example "mdi:battery".
   */
  virtual std::string icon();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// The MQTT sensor class uses this to register itself as a listener for new values.
  void add_new_value_callback(sensor_callback_t callback);

 protected:
  CallbackManager<void(float, int8_t)> callback_{};
};

class TemperatureSensor : public Sensor {
 public:
  std::string unit_of_measurement() override;
  std::string icon() override;
};

class HumiditySensor : public Sensor {
 public:
  std::string unit_of_measurement() override;
  std::string icon() override;
};

class VoltageSensor : public Sensor {
 public:
  std::string unit_of_measurement() override;
  std::string icon() override;
};

class DistanceSensor : public Sensor {
 public:
  std::string unit_of_measurement() override;
  std::string icon() override;
};

} // namespace sensor

} // namespace esphomelib


#endif //ESPHOMELIB_SENSOR_SENSOR_H
