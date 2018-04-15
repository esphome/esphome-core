//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

namespace esphomelib {

namespace binary_sensor {

/// typedef for binary_sensor callbacks. First parameter is new value.
using binary_callback_t = std::function<void(bool)>;

/** Base class for all binary_sensor-type classes.
 *
 * This class includes a callback that components such as MQTT can subscribe to for state changes.
 * The sub classes should notify the front-end of new states via the publish_state() method which
 * handles inverted inputs for you.
 */
class BinarySensor : public Nameable {
 public:
  explicit BinarySensor(const std::string &name);

  /** Set callback for state changes.
   *
   * @param callback The void(bool) callback.
   */
  virtual void add_on_state_callback(binary_callback_t &&callback);

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

  bool get_value() const;

  /// Set the Home Assistant device class (see esphomelib::binary_sensor::device_class)
  void set_device_class(const std::string &device_class);

  /// Get the device class for this binary sensor.
  std::string get_device_class();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Return whether all states of this binary sensor should be inverted.
  bool is_inverted() const;

 protected:
  CallbackManager<void(bool)> new_state_callback_{};
  bool inverted_{false};
  bool value_{false};
  bool first_value_{true};
  Optional<std::string> device_class_;
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //USE_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
