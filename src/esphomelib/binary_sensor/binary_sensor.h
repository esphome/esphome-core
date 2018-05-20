//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/automation.h"
#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

/// typedef for binary_sensor callbacks. First parameter is new value.
using binary_callback_t = std::function<void(bool)>;

class PressTrigger;
class ReleaseTrigger;
class ClickTrigger;
class DoubleClickTrigger;

/** Base class for all binary_sensor-type classes.
 *
 * This class includes a callback that components such as MQTT can subscribe to for state changes.
 * The sub classes should notify the front-end of new states via the publish_state() method which
 * handles inverted inputs for you.
 */
class BinarySensor : public Nameable {
 public:
  /** Construct a binary sensor with the specified name
   *
   * @param name Name of this binary sensor.
   */
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

  /// Get the current boolean value of this binary sensor.
  bool get_value() const;

  /// Manually set the Home Assistant device class (see esphomelib::binary_sensor::device_class)
  void set_device_class(const std::string &device_class);

  /// Get the device class for this binary sensor, using the manual override if specified.
  std::string get_device_class();

  PressTrigger *make_press_trigger();

  ReleaseTrigger *make_release_trigger();

  ClickTrigger *make_click_trigger(uint32_t min_length, uint32_t max_length);

  DoubleClickTrigger *make_double_click_trigger(uint32_t min_length, uint32_t max_length);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Return whether all states of this binary sensor should be inverted.
  bool is_inverted() const;

  bool value{false};

 protected:
  // ========== OVERRIDE METHODS ==========
  // (You'll only need this when creating your own custom sensor)
  /// Get the default device class for this sensor, or empty string for no default.
  virtual std::string device_class();

  CallbackManager<void(bool)> state_callback_{};
  bool inverted_{false};
  bool first_value_{true};
  optional<std::string> device_class_{}; ///< Stores the override of the device class
};

class PressTrigger : public Trigger<NoArg> {
 public:
  explicit PressTrigger(BinarySensor *parent);
};

class ReleaseTrigger : public Trigger<NoArg> {
 public:
  explicit ReleaseTrigger(BinarySensor *parent);
};

class ClickTrigger : public Trigger<NoArg> {
 public:
  explicit ClickTrigger(BinarySensor *parent, uint32_t min_length, uint32_t max_length);

 protected:
  uint32_t start_time_{0}; /// The millis() time when the click started.
  uint32_t min_length_; /// Minimum length of click. 0 means no minimum.
  uint32_t max_length_; /// Maximum length of click. 0 means no maximum.
};

class DoubleClickTrigger : public Trigger<NoArg> {
 public:
  explicit DoubleClickTrigger(BinarySensor *parent, uint32_t min_length, uint32_t max_length);

 protected:
  uint32_t start_time_{0};
  uint32_t end_time_{0};
  uint32_t min_length_; /// Minimum length of click. 0 means no minimum.
  uint32_t max_length_; /// Maximum length of click. 0 means no maximum.
};

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_BINARY_SENSOR_H
