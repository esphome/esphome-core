//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H

#include "esphomelib/component.h"

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
class BinarySensor {
 public:
  /** Set callback for state changes.
   *
   * @param callback The void(bool) callback.
   */
  virtual void set_on_new_state_callback(binary_callback_t callback);

  /// Set the inverted state of this binary sensor. If true, each published value will be inverted.
  void set_inverted(bool inverted);

  /** Publish a new state.
   *
   * Inverted input is handled by this method and sub-classes don't need to worry about inverting themselves.
   *
   * @param state The new state.
   */
  virtual void publish_state(bool state);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Return whether all states of this binary sensor should be inverted.
  bool is_inverted() const;

 protected:
  binary_callback_t new_state_callback_{nullptr};
  bool inverted_{false};
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
