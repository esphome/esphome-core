#ifndef ESPHOME_CLIMATEDEVICE_CLIMATEDEVICE_H
#define ESPHOME_CLIMATEDEVICE_CLIMATEDEVICE_H

#include "esphome/defines.h"

#ifdef USE_CLIMATEDEVICE

#include "esphome/component.h"
#include "esphome/automation.h"
#include "esphome/esppreferences.h"
#include "esphome/climatedevice/climatedevice_traits.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

/// Simple enum to represent the mode of a climate device.
enum ClimateDeviceMode {
  CLIMATEDEVICE_MODE_OFF = 0,   ///< The climate device is off.
  CLIMATEDEVICE_MODE_AUTO = 1,  ///< The climate device is in auto mode.
  CLIMATEDEVICE_MODE_COOL = 2,  ///< The climate device is in cool mode.
  CLIMATEDEVICE_MODE_HEAT = 3,  ///< The climate device is in heat mode.
};

template<typename... Ts> class ControlAction;
template<typename... Ts> class PublishAction;
template<typename... Ts> class CurrentTemperatureAction;
template<typename... Ts> class ModeCondition;

struct ClimateDeviceState {
  /// The mode of the climate device.
  ClimateDeviceMode mode{CLIMATEDEVICE_MODE_OFF};
  /// The target temperature.
  float target_temperature{NAN};
};

#ifdef USE_MQTT_CLIMATEDEVICE
class MQTTClimateDeviceComponent;
#endif

/** This class is climate device base class between the hardware backend and the MQTT frontend.
 *
 * A climate device has several variables that determine the current state: mode
 * (OFF, AUTO, HEAT, COOL), current_temperature, target_temperature and traits (what features are supported).
 * Both the frontend and the backend can register callbacks whenever a state is changed from the
 * frontend and whenever a state is actually changed and should be pushed to the frontend
 */
class ClimateDevice : public Nameable, public PollingComponent {
 public:
  /// Construct the climatedevice with name.
  explicit ClimateDevice(const std::string &name, uint32_t update_interval = 60000);

  /// Register a callback that will be called each time the mode changes is requested.
  void add_on_state_callback(std::function<void(ClimateDeviceState)> &&callback);
  /// Register a callback that will be called each time the mode changes.
  void add_on_publish_state_callback(std::function<void()> &&callback);
  /// Register a callback that will be called each time the current temperature changes
  /// (parameter is target_temperature - current_temperature).
  void add_on_current_temperature_callback(std::function<void(float)> &&callback);

  /// Get the traits of this climate device (i.e. what features it supports).
  const ClimateDeviceTraits &get_traits() const;
  /// Set the traits of this climate device (i.e. what features it supports).
  void set_traits(const ClimateDeviceTraits &traits);

  template<typename... Ts> ControlAction<Ts...> *make_control_action();
  template<typename... Ts> PublishAction<Ts...> *make_publish_action();
  template<typename... Ts> CurrentTemperatureAction<Ts...> *make_current_temperature_action();
  template<typename... Ts> ModeCondition<Ts...> *make_mode_condition();

  /// Update current temperature
  void set_current_temperature(float current_temperature);

  /// The current state of the climate device.
  ClimateDeviceState state;
  /// The current temperature.
  float current_temperature{NAN};

  /// Set the accuracy in decimals for the current temperature. Defaults to 1.
  void set_current_temperature_accuracy_decimals(int8_t accuracy_decimals);
  /// Get the accuracy in decimals for the current temperature.
  int8_t get_current_temperature_accuracy_decimals() const;
  /// Set the accuracy in decimals for the target temperature. Defaults to 0.
  void set_target_temperature_accuracy_decimals(int8_t accuracy_decimals);
  /// Get the accuracy in decimals for the target temperature.
  int8_t get_target_temperature_accuracy_decimals() const;
  /// Set the initial target temperature. Defaults to 21.
  void set_target_temperature_initial(float initial);
  /// Get the initial target temperature.
  float get_target_temperature_initial() const;
  /// Set the step size of the target temperature. Defaults to 1.0.
  void set_target_temperature_step(float step);
  /// Get the step size of the target temperature.
  float get_target_temperature_step() const;
  /// Set target temperature range. Defaults to 10-30.
  void set_target_temperature_range(float min, float max);
  /// Set supported modes.
  void set_modes(std::vector<ClimateDeviceMode> modes);

  class StateCall {
   public:
    explicit StateCall(ClimateDevice *device);

    ClimateDevice::StateCall &set_state(ClimateDeviceState state);
    ClimateDevice::StateCall &set_mode(ClimateDeviceMode mode);
    ClimateDevice::StateCall &set_mode(optional<ClimateDeviceMode> mode);
    ClimateDevice::StateCall &set_target_temperature(float target_temperature);

    void perform() const;
    void publish() const;

   protected:
    ClimateDeviceState validate_() const;
    ClimateDevice *const device_;
    optional<ClimateDeviceMode> mode_;
    optional<float> target_temperature_;
  };

  ClimateDevice::StateCall make_call();

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

#ifdef USE_MQTT_CLIMATEDEVICE
  MQTTClimateDeviceComponent *get_mqtt() const;
  void set_mqtt(MQTTClimateDeviceComponent *mqtt);
#endif

 protected:
  uint32_t hash_base() override;

  float target_temperature_initial_{21};
  float target_temperature_step_{1.0};
  int8_t current_temperature_accuracy_decimals_{1};
  int8_t target_temperature_accuracy_decimals_{0};
  ClimateDeviceTraits traits_{};
  CallbackManager<void(ClimateDeviceState)> state_callback_{};
  CallbackManager<void()> publish_state_callback_{};
  CallbackManager<void(float)> current_temperature_callback_{};
  ESPPreferenceObject rtc_;
#ifdef USE_MQTT_CLIMATEDEVICE
  MQTTClimateDeviceComponent *mqtt_{nullptr};
#endif
};

template<typename... Ts> class ControlAction : public Action<Ts...> {
 public:
  explicit ControlAction(ClimateDevice *device) : device_(device){};

  template<typename V> void set_mode(V value) { this->mode_ = value; }
  template<typename V> void set_target_temperature(V value) { this->target_temperature_ = value; }

  void play(Ts... x) override;

 protected:
  ClimateDevice *device_;
  TemplatableValue<ClimateDeviceMode, Ts...> mode_;
  TemplatableValue<float, Ts...> target_temperature_;
};
template<typename... Ts> class PublishAction : public Action<Ts...> {
 public:
  explicit PublishAction(ClimateDevice *device) : device_(device){};

  template<typename V> void set_mode(V value) { this->mode_ = value; }
  template<typename V> void set_target_temperature(V value) { this->target_temperature_ = value; }

  void play(Ts... x) override;

 protected:
  ClimateDevice *device_;
  TemplatableValue<ClimateDeviceMode, Ts...> mode_;
  TemplatableValue<float, Ts...> target_temperature_;
};
template<typename... Ts> class CurrentTemperatureAction : public Action<Ts...> {
 public:
  explicit CurrentTemperatureAction(ClimateDevice *device) : device_(device){};

  template<typename V> void set_current_temperature(V value) { this->current_temperature_ = value; }

  void play(Ts... x) override;

 protected:
  ClimateDevice *device_;
  TemplatableValue<float, Ts...> current_temperature_;
};
template<typename... Ts> class ModeCondition : public Condition<Ts...> {
 public:
  ModeCondition(ClimateDevice *parent) : parent_(parent){};

  void set_mode(ClimateDeviceMode mode) { this->mode_ = mode; }
  bool check(Ts... x) override;

 protected:
  ClimateDevice *parent_;
  ClimateDeviceMode mode_{CLIMATEDEVICE_MODE_OFF};
};

// =============== TEMPLATE DEFINITIONS ===============

template<typename... Ts> ControlAction<Ts...> *ClimateDevice::make_control_action() {
  return new ControlAction<Ts...>(this);
}
template<typename... Ts> void ControlAction<Ts...>::play(Ts... x) {
  auto call = this->device_->make_call();
  if (this->mode_.has_value()) {
    call.set_mode(this->mode_.value(x...));
  }
  if (this->target_temperature_.has_value()) {
    call.set_target_temperature(this->target_temperature_.value(x...));
  }
  call.perform();
  this->play_next(x...);
}
template<typename... Ts> PublishAction<Ts...> *ClimateDevice::make_publish_action() {
  return new PublishAction<Ts...>(this);
}
template<typename... Ts> void PublishAction<Ts...>::play(Ts... x) {
  auto call = this->device_->make_call();
  if (this->mode_.has_value()) {
    call.set_mode(this->mode_.value(x...));
  }
  if (this->target_temperature_.has_value()) {
    call.set_target_temperature(this->target_temperature_.value(x...));
  }
  call.publish();
  this->play_next(x...);
}
template<typename... Ts> CurrentTemperatureAction<Ts...> *ClimateDevice::make_current_temperature_action() {
  return new CurrentTemperatureAction<Ts...>(this);
}
template<typename... Ts> void CurrentTemperatureAction<Ts...>::play(Ts... x) {
  this->device_->set_current_temperature(this->current_temperature_.value(x...));
  this->play_next(x...);
}
template<typename... Ts> ModeCondition<Ts...> *ClimateDevice::make_mode_condition() {
  return new ModeCondition<Ts...>(this);
}
template<typename... Ts> bool ModeCondition<Ts...>::check(Ts... x) { return this->mode_ == this->parent_->state.mode; }

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#include "esphome/climatedevice/mqtt_climatedevice_component.h"

#endif  // USE_CLIMATEDEVICE

#endif  // ESPHOME_CLIMATEDEVICE_CLIMATEDEVICE_H
