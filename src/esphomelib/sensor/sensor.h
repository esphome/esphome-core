#ifndef ESPHOMELIB_SENSOR_SENSOR_H
#define ESPHOMELIB_SENSOR_SENSOR_H

#include <functional>
#include <list>

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/automation.h"
#include "esphomelib/sensor/filter.h"
#include "esphomelib/defines.h"

#ifdef USE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using sensor_callback_t = std::function<void(float)>;

class MQTTSensorComponent;
class SensorValueTrigger;
class RawSensorValueTrigger;
class ValueRangeTrigger;

/** Base-class for all sensors.
 *
 * A sensor has unit of measurement and can use push_new_value to send out a new value with the specified accuracy.
 */
class Sensor : public Nameable {
 public:
  explicit Sensor(const std::string &name);

  /** Manually set the unit of measurement of this sensor. By default the sensor's default defined by
   * unit_of_measurement() is used.
   *
   * @param unit_of_measurement The unit of measurement, "" to disable.
   */
  void set_unit_of_measurement(const std::string &unit_of_measurement);

  /** Manually set the icon of this sensor. By default the sensor's default defined by icon() is used.
  *
  * @param icon The icon, for example "mdi:flash". "" to disable.
  */
  void set_icon(const std::string &icon);

  /** Manually set the accuracy in decimals for this sensor. By default, the sensor's default defined by
   * accuracy_decimals() is used.
   *
   * @param accuracy_decimals The accuracy decimal that should be used.
   */
  void set_accuracy_decimals(int8_t accuracy_decimals);

  /// Add a filter to the filter chain. Will be appended to the back.
  void add_filter(Filter *filter);

  /** Add a list of vectors to the back of the filter chain.
   *
   * This may look like:
   *
   * sensor->add_filters({
   *   LambdaFilter([&](float value) -> optional<float> { return 42/value; }),
   *   OffsetFilter(1),
   *   SlidingWindowMovingAverageFilter(15, 15), // average over last 15 values
   * });
   */
  void add_filters(const std::list<Filter *> & filters);

  /// Clear the filters and replace them by filters.
  void set_filters(const std::list<Filter *> & filters);

  /** Add a lambda filter to the back of the filter chain.
   *
   * For example:
   * sensor->add_lambda_filter([](float value) -> optional<float> {
   *   return value * 42;
   * });
   *
   * If you return an unset Optional, the value will be discarded and no
   * filters after this one will get the value.
   */
  void add_lambda_filter(lambda_filter_t filter);

  /** Helper to add a simple offset filter to the back of the filter chain.
   *
   * This can be used to easily correct for sensors that have a small offset
   * in their value reporting.
   *
   * @param offset The offset that will be added to each value.
   */
  void add_offset_filter(float offset);

  /** Helper to add a simple multiply filter to the back of the filter chain.
   *
   * Each value will be multiplied by this multiplier. Can be used to convert units
   * easily. For example converting "pulses/min" to a more reasonable unit like kW.
   *
   * @param multiplier The multiplier each value will be multiplied with.
   */
  void add_multiply_filter(float multiplier);

  /** Helper to add a simple filter that aborts the filter chain every time it receives a specific value.
   *
   * @param values_to_filter_out The value that should be filtered out.
   */
  void add_filter_out_value_filter(float values_to_filter_out);

  /// Helper to make adding sliding window moving average filters a bit easier.
  void add_sliding_window_average_filter(size_t window_size, size_t send_every);

  /// Helper to make adding exponential decay average filters a bit easier.
  void add_exponential_moving_average_filter(float alpha, size_t send_every);

  /// Clear the entire filter chain.
  void clear_filters();

  /// Get the latest filtered value from this sensor.
  float get_value() const;
  /// Get the latest raw value from this sensor.
  float get_raw_value() const;

  /// Get the accuracy in decimals used by this MQTT Sensor, first checks override, then sensor.
  int8_t get_accuracy_decimals();

  /// Get the unit of measurements advertised to Home Assistant. First checks override, then sensor.
  std::string get_unit_of_measurement();

  /// Get the icon advertised to Home Assistant.
  std::string get_icon();

  // ========== OVERRIDE METHODS ==========
  // (You'll only need this when creating your own custom sensor)
  /** Push a new value to the MQTT front-end.
   *
   * Note that you should publish the raw value here, i.e. without any rounding as the user
   * can later override this accuracy.
   *
   * @param value The floating point value.
   */
  void push_new_value(float value);

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

  /// Return with which interval the sensor is polled. Return 0 for non-polling mode.
  virtual uint32_t update_interval();

  /// Return the accuracy in decimals for this sensor.
  virtual int8_t accuracy_decimals();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Add a callback that will be called every time a filtered value arrives.
  void add_on_value_callback(sensor_callback_t callback);
  /// Add a callback that will be called every time the sensor sends a raw value.
  void add_on_raw_value_callback(sensor_callback_t callback);

  /** A unique ID for this sensor, empty for no unique id. See unique ID requirements:
   * https://developers.home-assistant.io/docs/en/entity_registry_index.html#unique-id-requirements
   *
   * @return The unique id as a string.
   */
  virtual std::string unique_id();

  SensorValueTrigger *make_value_trigger();
  RawSensorValueTrigger *make_raw_value_trigger();
  ValueRangeTrigger *make_value_range_trigger();

  float value{NAN}; ///< Stores the last filtered value. Public because of lambdas.
  float raw_value{NAN}; ///< Stores the last raw value. Public because of lambdas.

  /// Return whether this sensor has gotten a full value (that passed through all filters) yet.
  bool has_value() const;

 protected:
  friend Filter;
  friend MQTTSensorComponent;

  void send_value_to_frontend(float value);

  CallbackManager<void(float)> raw_callback_; ///< Storage for raw value callbacks.
  CallbackManager<void(float)> callback_; ///< Storage for filtered value callbacks.
  optional<std::string> unit_of_measurement_; ///< Override the unit of measurement
  optional<std::string> icon_; /// Override the icon advertised to Home Assistant, otherwise sensor's icon will be used.
  optional<int8_t> accuracy_decimals_; ///< Override the accuracy in decimals, otherwise the sensor's values will be used.
  Filter *filter_list_{nullptr}; ///< Store all active filters.
  bool has_value_{false};
};

class PollingSensorComponent : public PollingComponent, public Sensor {
 public:
  explicit PollingSensorComponent(const std::string &name, uint32_t update_interval);

  uint32_t update_interval() override;
};

template<int8_t default_accuracy_decimals, const char *default_icon, const char *default_unit_of_measurement>
class EmptySensor;

template<int8_t default_accuracy_decimals, const char *default_icon, const char *default_unit_of_measurement>
class EmptySensor : public Sensor {
 public:
  explicit EmptySensor(const std::string &name)
      : Sensor(name) {

  }

  std::string unit_of_measurement() override {
    return default_unit_of_measurement;
  }
  std::string icon() override {
    return default_icon;
  }
  int8_t accuracy_decimals() override {
    return default_accuracy_decimals;
  }
};

template<int8_t default_accuracy_decimals, const char *default_icon, const char *default_unit_of_measurement,
    class ParentType = PollingComponent>
class EmptyPollingParentSensor;

template<int8_t default_accuracy_decimals, const char *default_icon, const char *default_unit_of_measurement,
    class ParentType>
class EmptyPollingParentSensor
    : public EmptySensor<default_accuracy_decimals, default_icon, default_unit_of_measurement> {
 public:
  EmptyPollingParentSensor(const std::string &name, ParentType *parent)
    : EmptySensor<default_accuracy_decimals, default_icon, default_unit_of_measurement>(name), parent_(parent) {

  }

  uint32_t update_interval() override {
    return parent_->get_update_interval();
  }

 protected:
  ParentType *parent_;
};

class SensorValueTrigger : public Trigger<float> {
 public:
  explicit SensorValueTrigger(Sensor *parent);
};

class RawSensorValueTrigger : public Trigger<float> {
 public:
  explicit RawSensorValueTrigger(Sensor *parent);
};

class ValueRangeTrigger : public Trigger<float> {
 public:
  explicit ValueRangeTrigger(Sensor *parent);

  void set_min(std::function<float(float)> &&min);
  void set_min(float min);
  void set_max(std::function<float(float)> &&max);
  void set_max(float max);

 protected:
  bool previous_in_range_{false};
  TemplatableValue<float, float> min_{NAN};
  TemplatableValue<float, float> max_{NAN};
};

extern const char ICON_EMPTY[];
extern const char ICON_WATER_PERCENT[];
extern const char ICON_GAUGE[];
extern const char ICON_FLASH[];
extern const char ICON_SCREEN_ROTATION[];
extern const char ICON_BRIEFCASE_DOWNLOAD[];
extern const char ICON_BRIGHTNESS_5[];
extern const char ICON_GAS_CYLINDER[];
extern const char ICON_PERIODIC_TABLE_CO2[];
extern const char ICON_MAGNET[];
extern const char ICON_THERMOMETER[];
extern const char ICON_LIGHTBULB[];
extern const char ICON_BATTERY[];
extern const char ICON_FLOWER[];
extern const char ICON_CHEMICAL_WEAPON[];

extern const char UNIT_C[];
extern const char UNIT_PERCENT[];
extern const char UNIT_HPA[];
extern const char UNIT_V[];
extern const char UNIT_DEGREES_PER_SECOND[];
extern const char UNIT_M_PER_S_SQUARED[];
extern const char UNIT_LX[];
extern const char UNIT_OHM[];
extern const char UNIT_PPM[];
extern const char UNIT_A[];
extern const char UNIT_W[];
extern const char UNIT_UT[];
extern const char UNIT_DEGREES[];
extern const char UNIT_K[];
extern const char UNIT_MICROSIEMENS_PER_CENTIMETER[];
extern const char UNIT_MICROGRAMS_PER_CUBIC_METER[];

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR

#endif //ESPHOMELIB_SENSOR_SENSOR_H
