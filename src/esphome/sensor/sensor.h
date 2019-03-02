#ifndef ESPHOME_SENSOR_SENSOR_H
#define ESPHOME_SENSOR_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_SENSOR

#include <functional>
#include <list>

#include "esphome/component.h"
#include "esphome/helpers.h"
#include "esphome/automation.h"
#include "esphome/sensor/filter.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class MQTTSensorComponent;
class SensorStateTrigger;
class SensorRawStateTrigger;
class ValueRangeTrigger;
template<typename... Ts> class SensorInRangeCondition;
template<typename... Ts> class SensorPublishAction;

#define LOG_SENSOR(prefix, type, obj) \
  if (obj != nullptr) { \
    ESP_LOGCONFIG(TAG, prefix type " '%s'", obj->get_name().c_str()); \
    ESP_LOGCONFIG(TAG, prefix "  Unit of Measurement: '%s'", obj->get_unit_of_measurement().c_str()); \
    ESP_LOGCONFIG(TAG, prefix "  Accuracy Decimals: %d", obj->get_accuracy_decimals()); \
    if (!obj->get_icon().empty()) { \
      ESP_LOGCONFIG(TAG, prefix "  Icon: '%s'", obj->get_icon().c_str()); \
    } \
    if (!obj->unique_id().empty()) { \
      ESP_LOGV(TAG, prefix "  Unique ID: '%s'", obj->unique_id().c_str()); \
    } \
  }

#ifdef USE_MQTT_SENSOR
class MQTTSensorComponent;
#endif

/** Base-class for all sensors.
 *
 * A sensor has unit of measurement and can use publish_state to send out a new value with the specified accuracy.
 */
class Sensor : public Nameable {
 public:
  explicit Sensor();

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
  void add_filters(const std::vector<Filter *> &filters);

  /// Clear the filters and replace them by filters.
  void set_filters(const std::vector<Filter *> &filters);

  /// Clear the entire filter chain.
  void clear_filters();

  /// Getter-syntax for .value. Please use .state instead.
  float get_value() const ESPDEPRECATED(".value is deprecated, please use .state");
  /// Getter-syntax for .state.
  float get_state() const;
  /// Getter-syntax for .raw_value. Please use .raw_state instead.
  float get_raw_value() const ESPDEPRECATED(".raw_value is deprecated, please use .raw_state");
  /// Getter-syntax for .raw_state
  float get_raw_state() const;

  /// Get the accuracy in decimals. Uses the manual override if specified or the default value instead.
  int8_t get_accuracy_decimals();

  /// Get the unit of measurement. Uses the manual override if specified or the default value instead.
  std::string get_unit_of_measurement();

  /// Get the Home Assistant Icon. Uses the manual override if specified or the default value instead.
  std::string get_icon();

  /** Publish a new state to the front-end.
   *
   * First, the new state will be assigned to the raw_value. Then it's passed through all filters
   * until it finally lands in the .value member variable and a callback is issued.
   *
   * @param state The state as a floating point number.
   */
  void publish_state(float state);

  /** Push a new value to the MQTT front-end.
   *
   * Note: deprecated, please use publish_state.
   */
  void push_new_value(float state) ESPDEPRECATED("push_new_value is deprecated. Please use .publish_state instead");

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Add a callback that will be called every time a filtered value arrives.
  void add_on_state_callback(std::function<void(float)> &&callback);
  /// Add a callback that will be called every time the sensor sends a raw value.
  void add_on_raw_state_callback(std::function<void(float)> &&callback);

  SensorStateTrigger *make_state_trigger();
  SensorRawStateTrigger *make_raw_state_trigger();
  ValueRangeTrigger *make_value_range_trigger();
  template<typename... Ts> SensorInRangeCondition<Ts...> *make_sensor_in_range_condition();
  template<typename... Ts> SensorPublishAction<Ts...> *make_sensor_publish_action();

  union {
    /** This member variable stores the last state that has passed through all filters.
     *
     * On startup, when no state is available yet, this is NAN (not-a-number) and the validity
     * can be checked using has_state().
     *
     * This is exposed through a member variable for ease of use in esphome lambdas.
     */
    float state;

    /// This is an alias for .state and kept here for compatability reasons.
    ESPDEPRECATED(".value is deprecated. Please use .state instead") float value;
  };

  union {
    /** This member variable stores the current raw state of the sensor. Unlike .state,
     * this will be updated immediately when publish_state is called.
     */
    float raw_state;

    /// This is an alias for .raw_state and kept here for compatability reasons.
    ESPDEPRECATED(".raw_value is deprecated. Please use .raw_state instead") float raw_value;
  };

  /// Return whether this sensor has gotten a full state (that passed through all filters) yet.
  bool has_state() const;

  /** A unique ID for this sensor, empty for no unique id. See unique ID requirements:
   * https://developers.home-assistant.io/docs/en/entity_registry_index.html#unique-id-requirements
   *
   * @return The unique id as a string.
   */
  virtual std::string unique_id();

  /// Return with which interval the sensor is polled. Return 0 for non-polling mode.
  virtual uint32_t update_interval();

  /// Calculate the expected update interval for values that pass through all filters.
  uint32_t calculate_expected_filter_update_interval();

  void internal_send_state_to_frontend(float state);

#ifdef USE_MQTT_SENSOR
  MQTTSensorComponent *get_mqtt() const;
  void set_mqtt(MQTTSensorComponent *mqtt);
#endif

 protected:
  /** Override this to set the Home Assistant unit of measurement for this sensor.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this sensor, for example "°C".
   */
  virtual std::string unit_of_measurement();  // NOLINT

  /** Override this to set the Home Assistant icon for this sensor.
   *
   * Return "" to disable this feature.
   *
   * @return The icon of this sensor, for example "mdi:battery".
   */
  virtual std::string icon();  // NOLINT

  /// Return the accuracy in decimals for this sensor.
  virtual int8_t accuracy_decimals();  // NOLINT

  uint32_t hash_base() override;

  CallbackManager<void(float)> raw_callback_;  ///< Storage for raw state callbacks.
  CallbackManager<void(float)> callback_;      ///< Storage for filtered state callbacks.
  optional<std::string> unit_of_measurement_;  ///< Override the unit of measurement
  optional<std::string>
      icon_;  /// Override the icon advertised to Home Assistant, otherwise sensor's icon will be used.
  optional<int8_t>
      accuracy_decimals_;         ///< Override the accuracy in decimals, otherwise the sensor's values will be used.
  Filter *filter_list_{nullptr};  ///< Store all active filters.
  bool has_state_{false};

#ifdef USE_MQTT_SENSOR
  MQTTSensorComponent *mqtt_{nullptr};
#endif
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
  explicit EmptySensor(const std::string &name) : Sensor(name) {}

  std::string unit_of_measurement() override { return default_unit_of_measurement; }
  std::string icon() override { return default_icon; }
  int8_t accuracy_decimals() override { return default_accuracy_decimals; }
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
      : EmptySensor<default_accuracy_decimals, default_icon, default_unit_of_measurement>(name), parent_(parent) {}

  uint32_t update_interval() override { return parent_->get_update_interval(); }

 protected:
  ParentType *parent_;
};

class SensorStateTrigger : public Trigger<float> {
 public:
  explicit SensorStateTrigger(Sensor *parent);
};

class SensorRawStateTrigger : public Trigger<float> {
 public:
  explicit SensorRawStateTrigger(Sensor *parent);
};

template<typename... Ts> class SensorPublishAction : public Action<Ts...> {
 public:
  SensorPublishAction(Sensor *sensor);
  template<typename V> void set_state(V state) { this->state_ = state; }
  void play(Ts... x) override;

 protected:
  Sensor *sensor_;
  TemplatableValue<float, Ts...> state_;
};

class ValueRangeTrigger : public Trigger<float>, public Component {
 public:
  explicit ValueRangeTrigger(Sensor *parent);

  template<typename V> void set_min(V min) { this->min_ = min; }
  template<typename V> void set_max(V max) { this->max_ = max; }

  void setup() override;
  float get_setup_priority() const override;

 protected:
  void on_state_(float state);

  Sensor *parent_;
  ESPPreferenceObject rtc_;
  bool previous_in_range_{false};
  TemplatableValue<float, float> min_{NAN};
  TemplatableValue<float, float> max_{NAN};
};

template<typename... Ts> class SensorInRangeCondition : public Condition<Ts...> {
 public:
  SensorInRangeCondition(Sensor *parent);

  void set_min(float min);
  void set_max(float max);
  bool check(Ts... x) override;

 protected:
  Sensor *parent_;
  float min_{NAN};
  float max_{NAN};
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

template<typename... Ts> SensorInRangeCondition<Ts...> *Sensor::make_sensor_in_range_condition() {
  return new SensorInRangeCondition<Ts...>(this);
}
template<typename... Ts> SensorInRangeCondition<Ts...>::SensorInRangeCondition(Sensor *parent) : parent_(parent) {}
template<typename... Ts> void SensorInRangeCondition<Ts...>::set_min(float min) { this->min_ = min; }
template<typename... Ts> void SensorInRangeCondition<Ts...>::set_max(float max) { this->max_ = max; }
template<typename... Ts> bool SensorInRangeCondition<Ts...>::check(Ts... x) {
  const float state = this->parent_->state;
  if (isnan(this->min_)) {
    return state <= this->max_;
  } else if (isnan(this->max_)) {
    return state >= this->min_;
  } else {
    return this->min_ <= state && state <= this->max_;
  }
}
template<typename... Ts> SensorPublishAction<Ts...>::SensorPublishAction(Sensor *sensor) : sensor_(sensor) {}
template<typename... Ts> void SensorPublishAction<Ts...>::play(Ts... x) {
  this->sensor_->publish_state(this->state_.value(x...));
  this->play_next(x...);
}
template<typename... Ts> SensorPublishAction<Ts...> *Sensor::make_sensor_publish_action() {
  return new SensorPublishAction<Ts...>(this);
}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#include "esphome/sensor/mqtt_sensor_component.h"

#endif  // USE_SENSOR

#endif  // ESPHOME_SENSOR_SENSOR_H
