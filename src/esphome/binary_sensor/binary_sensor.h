#ifndef ESPHOME_BINARY_SENSOR_BINARY_SENSOR_H
#define ESPHOME_BINARY_SENSOR_BINARY_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphome/component.h"
#include "esphome/helpers.h"
#include "esphome/automation.h"
#include "esphome/binary_sensor/filter.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

/// typedef for binary_sensor callbacks. First parameter is new value.

class PressTrigger;
class ReleaseTrigger;
class ClickTrigger;
class DoubleClickTrigger;
class MultiClickTrigger;
class StateTrigger;
template<typename... Ts> class BinarySensorCondition;
class Filter;
template<typename... Ts> class BinarySensorPublishAction;

struct MultiClickTriggerEvent {
  bool state;
  uint32_t min_length;
  uint32_t max_length;
};

#define LOG_BINARY_SENSOR(prefix, type, obj) \
  if (obj != nullptr) { \
    ESP_LOGCONFIG(TAG, prefix type " '%s'", obj->get_name().c_str()); \
    if (!obj->get_device_class().empty()) { \
      ESP_LOGCONFIG(TAG, prefix "  Device Class: '%s'", obj->get_device_class().c_str()); \
    } \
  }

#ifdef USE_MQTT_BINARY_SENSOR
class MQTTBinarySensorComponent;
#endif

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
  explicit BinarySensor();

  /** Add a callback to be notified of state changes.
   *
   * @param callback The void(bool) callback.
   */
  void add_on_state_callback(std::function<void(bool)> &&callback);

  /** Publish a new state to the front-end.
   *
   * @param state The new state.
   */
  void publish_state(bool state);

  /** Publish the initial state, this will not make the callback manager send callbacks
   * and is meant only for the initial state on boot.
   *
   * @param state The new state.
   */
  void publish_initial_state(bool state);

  union {
    /// The current reported state of the binary sensor.
    bool state;
    ESPDEPRECATED(".value is deprecated, please use .state instead") bool value;
  };

  /// Manually set the Home Assistant device class (see binary_sensor::device_class)
  void set_device_class(const std::string &device_class);

  /// Get the device class for this binary sensor, using the manual override if specified.
  std::string get_device_class();

  PressTrigger *make_press_trigger();
  ReleaseTrigger *make_release_trigger();
  ClickTrigger *make_click_trigger(uint32_t min_length, uint32_t max_length);
  DoubleClickTrigger *make_double_click_trigger(uint32_t min_length, uint32_t max_length);
  MultiClickTrigger *make_multi_click_trigger(const std::vector<MultiClickTriggerEvent> &timing);
  StateTrigger *make_state_trigger();
  template<typename... Ts> BinarySensorCondition<Ts...> *make_binary_sensor_is_on_condition(uint32_t for_time = 0);
  template<typename... Ts> BinarySensorCondition<Ts...> *make_binary_sensor_is_off_condition(uint32_t for_time = 0);
  template<typename... Ts> BinarySensorPublishAction<Ts...> *make_binary_sensor_publish_action();

  void add_filter(Filter *filter);
  void add_filters(std::vector<Filter *> filters);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void send_state_internal(bool state, bool is_initial);

  /// Return whether this binary sensor has outputted a state.
  bool has_state() const;

  virtual bool is_status_binary_sensor() const;

#ifdef USE_MQTT_BINARY_SENSOR
  MQTTBinarySensorComponent *get_mqtt() const;
  void set_mqtt(MQTTBinarySensorComponent *mqtt);
#endif

  // ========== OVERRIDE METHODS ==========
  // (You'll only need this when creating your own custom binary sensor)
  /// Get the default device class for this sensor, or empty string for no default.
  virtual std::string device_class();

 protected:
  uint32_t hash_base() override;

  CallbackManager<void(bool)> state_callback_{};
  optional<std::string> device_class_{};  ///< Stores the override of the device class
  Filter *filter_list_{nullptr};
  bool has_state_{false};
  Deduplicator<bool> publish_dedup_;

#ifdef USE_MQTT_BINARY_SENSOR
  MQTTBinarySensorComponent *mqtt_{nullptr};
#endif
};

class PressTrigger : public Trigger<> {
 public:
  explicit PressTrigger(BinarySensor *parent);
};

class ReleaseTrigger : public Trigger<> {
 public:
  explicit ReleaseTrigger(BinarySensor *parent);
};

class ClickTrigger : public Trigger<> {
 public:
  explicit ClickTrigger(BinarySensor *parent, uint32_t min_length, uint32_t max_length);

 protected:
  uint32_t start_time_{0};  /// The millis() time when the click started.
  uint32_t min_length_;     /// Minimum length of click. 0 means no minimum.
  uint32_t max_length_;     /// Maximum length of click. 0 means no maximum.
};

class DoubleClickTrigger : public Trigger<> {
 public:
  explicit DoubleClickTrigger(BinarySensor *parent, uint32_t min_length, uint32_t max_length);

 protected:
  uint32_t start_time_{0};
  uint32_t end_time_{0};
  uint32_t min_length_;  /// Minimum length of click. 0 means no minimum.
  uint32_t max_length_;  /// Maximum length of click. 0 means no maximum.
};

class MultiClickTrigger : public Trigger<>, public Component {
 public:
  explicit MultiClickTrigger(BinarySensor *parent, const std::vector<MultiClickTriggerEvent> &timing);

  void setup() override;

  float get_setup_priority() const override;

  void set_invalid_cooldown(uint32_t invalid_cooldown);

 protected:
  void on_state_(bool state);
  void schedule_cooldown_();
  void schedule_is_valid_(uint32_t min_length);
  void schedule_is_not_valid_(uint32_t max_length);
  void trigger_();

  BinarySensor *parent_;
  std::vector<MultiClickTriggerEvent> timing_;
  uint32_t invalid_cooldown_{1000};
  optional<size_t> at_index_{};
  bool last_state_{false};
  bool is_in_cooldown_{false};
  bool is_valid_{false};
};

class StateTrigger : public Trigger<bool> {
 public:
  explicit StateTrigger(BinarySensor *parent);
};

template<typename... Ts> class BinarySensorCondition : public Condition<Ts...> {
 public:
  BinarySensorCondition(BinarySensor *parent, bool state, uint32_t for_time = 0);
  bool check(Ts... x) override;

 protected:
  BinarySensor *parent_;
  bool state_;
  uint32_t last_state_time_{0};
  uint32_t for_time_{0};
};

template<typename... Ts> class BinarySensorPublishAction : public Action<Ts...> {
 public:
  BinarySensorPublishAction(BinarySensor *sensor);
  template<typename V> void set_state(V value) { this->state_ = value; }
  void play(Ts... x) override;

 protected:
  BinarySensor *sensor_;
  TemplatableValue<bool, Ts...> state_;
};

template<typename... Ts>
BinarySensorCondition<Ts...>::BinarySensorCondition(BinarySensor *parent, bool state, uint32_t for_time)
    : parent_(parent), state_(state), for_time_(for_time) {
  parent->add_on_state_callback([this](bool state) { this->last_state_time_ = millis(); });
}
template<typename... Ts> bool BinarySensorCondition<Ts...>::check(Ts... x) {
  if (this->parent_->state != this->state_)
    return false;

  return millis() - this->last_state_time_ >= this->for_time_;
}

template<typename... Ts>
BinarySensorCondition<Ts...> *BinarySensor::make_binary_sensor_is_on_condition(uint32_t for_time) {
  return new BinarySensorCondition<Ts...>(this, true, for_time);
}
template<typename... Ts>
BinarySensorCondition<Ts...> *BinarySensor::make_binary_sensor_is_off_condition(uint32_t for_time) {
  return new BinarySensorCondition<Ts...>(this, false, for_time);
}
template<typename... Ts>
BinarySensorPublishAction<Ts...>::BinarySensorPublishAction(BinarySensor *sensor) : sensor_(sensor) {}
template<typename... Ts> void BinarySensorPublishAction<Ts...>::play(Ts... x) {
  auto val = this->state_.value(x...);
  this->sensor_->publish_state(val);
  this->play_next(x...);
}
template<typename... Ts> BinarySensorPublishAction<Ts...> *BinarySensor::make_binary_sensor_publish_action() {
  return new BinarySensorPublishAction<Ts...>(this);
}

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#include "esphome/binary_sensor/mqtt_binary_sensor_component.h"

#endif  // USE_BINARY_SENSOR

#endif  // ESPHOME_BINARY_SENSOR_BINARY_SENSOR_H
