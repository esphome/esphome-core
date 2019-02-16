#ifndef ESPHOME_DEEP_SLEEP_COMPONENT_H
#define ESPHOME_DEEP_SLEEP_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_DEEP_SLEEP

#include "esphome/component.h"
#include "esphome/helpers.h"
#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

#ifdef ARDUINO_ARCH_ESP32

/** The values of this enum define what should be done if deep sleep is set up with a wakeup pin on the ESP32
 * and the scenario occurs that the wakeup pin is already in the wakeup state.
 */
enum WakeupPinMode {
  WAKEUP_PIN_MODE_IGNORE = 0, ///< Ignore the fact that we will wake up when going into deep sleep.
  WAKEUP_PIN_MODE_KEEP_AWAKE, ///< As long as the wakeup pin is still in the wakeup state, keep awake.

  /** Automatically invert the wakeup level. For example if we were set up to wake up on HIGH, but the pin
   * is already high when attempting to enter deep sleep, re-configure deep sleep to wake up on LOW level.
   */
  WAKEUP_PIN_MODE_INVERT_WAKEUP,
};

struct Ext1Wakeup {
  uint64_t mask;
  esp_sleep_ext1_wakeup_mode_t wakeup_mode;
};

#endif

template<typename T>
class EnterDeepSleepAction;

template<typename T>
class PreventDeepSleepAction;

/** This component allows setting up the node to go into deep sleep mode to conserve battery.
 *
 * To set this component up, first set *when* the deep sleep should trigger using set_run_cycles
 * and set_run_duration, then set how long the deep sleep should last using set_sleep_duration and optionally
 * on the ESP32 set_wakeup_pin.
 */
class DeepSleepComponent : public Component {
 public:
  /// Set the duration in ms the component should sleep once it's in deep sleep mode.
  void set_sleep_duration(uint32_t time_ms);
#ifdef ARDUINO_ARCH_ESP32
  /** Set the pin to wake up to on the ESP32 once it's in deep sleep mode.
   * Use the inverted property to set the wakeup level.
   */
  void set_wakeup_pin(const GPIOInputPin &pin);

  void set_wakeup_pin_mode(WakeupPinMode wakeup_pin_mode);

  void set_ext1_wakeup(Ext1Wakeup ext1_wakeup);
#endif
  /// Set a duration in ms for how long the code should run before entering deep sleep mode.
  void set_run_duration(uint32_t time_ms);

  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_loop_priority() const override;
  float get_setup_priority() const override;

  /// Helper to enter deep sleep mode
  void begin_sleep_(bool manual = false);

  template<typename T>
  EnterDeepSleepAction<T> *make_enter_deep_sleep_action();

  template<typename T>
  PreventDeepSleepAction<T> *make_prevent_deep_sleep_action();

  void prevent_deep_sleep();

 protected:
  optional<uint64_t> sleep_duration_;
#ifdef ARDUINO_ARCH_ESP32
  optional<GPIOPin *> wakeup_pin_;
  WakeupPinMode  wakeup_pin_mode_{WAKEUP_PIN_MODE_IGNORE};
  optional<Ext1Wakeup> ext1_wakeup_;
#endif
  optional<uint32_t> run_duration_;
  bool next_enter_deep_sleep_{false};
  bool prevent_{false};
};

extern bool global_has_deep_sleep;

template<typename T>
class EnterDeepSleepAction : public Action<T> {
 public:
  EnterDeepSleepAction(DeepSleepComponent *deep_sleep);

  void play(T x) override;
 protected:
  DeepSleepComponent *deep_sleep_;
};

template<typename T>
class PreventDeepSleepAction : public Action<T> {
 public:
  PreventDeepSleepAction(DeepSleepComponent *deep_sleep);

  void play(T x) override;
 protected:
  DeepSleepComponent *deep_sleep_;
};

template<typename T>
EnterDeepSleepAction<T>::EnterDeepSleepAction(DeepSleepComponent *deep_sleep) : deep_sleep_(deep_sleep) {}

template<typename T>
void EnterDeepSleepAction<T>::play(T x) {
  this->deep_sleep_->begin_sleep_(true);
  // no need to call play_next. We should be done with execution by now.
}

template<typename T>
EnterDeepSleepAction<T> *DeepSleepComponent::make_enter_deep_sleep_action() {
  return new EnterDeepSleepAction<T>(this);
}

template<typename T>
PreventDeepSleepAction<T>::PreventDeepSleepAction(DeepSleepComponent *deep_sleep)
    : deep_sleep_(deep_sleep) {

}
template<typename T>
void PreventDeepSleepAction<T>::play(T x) {
  this->deep_sleep_->prevent_deep_sleep();
  this->play_next(x);
}

template<typename T>
PreventDeepSleepAction<T> *DeepSleepComponent::make_prevent_deep_sleep_action() {
  return new PreventDeepSleepAction<T>(this);
}

ESPHOME_NAMESPACE_END

#endif //USE_DEEP_SLEEP

#endif //ESPHOME_DEEP_SLEEP_COMPONENT_H
