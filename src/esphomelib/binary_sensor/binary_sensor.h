//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"

namespace esphomelib {

namespace binary_sensor {

/// Home Assistant device classes. See <a href="https://home-assistant.io/components/binary_sensor/">Binary Sensor</a>.
namespace device_class {

const std::string NONE = "";
const std::string BATTERY = "";
const std::string COLD = "cold";
const std::string CONNECTIVITY = "connectivity";
const std::string DOOR = "door";
const std::string GARAGE_DOOR = "garage_door";
const std::string GAS = "gas";
const std::string HEAT = "heat";
const std::string LIGHT = "light";
const std::string LOCK = "lock";
const std::string MOISTURE = "moisture";
const std::string MOTION = "motion";
const std::string MOVING = "moving";
const std::string OCCUPANCY = "occupancy";
const std::string OPENING = "opening";
const std::string PLUG = "plug";
const std::string POWER = "power";
const std::string PRESENCE = "presence";
const std::string SAFETY = "safety";
const std::string SMOKE = "smoke";
const std::string SOUND = "sound";
const std::string VIBRATION = "vibration";
const std::string WINDOW = "window";

}

/// typedef for binary_sensor callbacks. First parameter is new value.
using binary_callback_t = std::function<void(bool)>;

/** Base class for all binary_sensor-type classes.
 *
 * This class includes a callback that components such as MQTT can subscribe to for state changes.
 * The sub classes should notify the front-end of new states via the publish_state() method which
 * handles inverted inputs for you.
 */
class BinarySensor {
 public:
  /** Set callback for state changes.
   *
   * @param callback The void(bool) callback.
   */
  virtual void add_on_new_state_callback(binary_callback_t callback);

  /// Set the inverted state of this binary sensor. If true, each published value will be inverted.
  void set_inverted(bool inverted);

  /** Publish a new state.
   *
   * Inverted input is handled by this method and sub-classes don't need to worry about inverting themselves.
   *
   * @param state The new state.
   */
  virtual void publish_state(bool state);

  /// Get the default device class for this sensor, or empty string for no default.
  virtual std::string device_class();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Return whether all states of this binary sensor should be inverted.
  bool is_inverted() const;

 protected:
  CallbackManager<void(bool)> new_state_callback_{};
  bool inverted_{false};
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
