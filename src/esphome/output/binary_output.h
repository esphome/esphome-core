#ifndef ESPHOME_OUTPUT_BINARY_OUTPUT_H
#define ESPHOME_OUTPUT_BINARY_OUTPUT_H

#include "esphome/defines.h"

#ifdef USE_OUTPUT

#include "esphome/automation.h"
#include "esphome/power_supply_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

template<typename... Ts> class TurnOffAction;
template<typename... Ts> class TurnOnAction;

#define LOG_BINARY_OUTPUT(this) \
  if (this->inverted_) { \
    ESP_LOGCONFIG(TAG, "  Inverted: YES"); \
  }

/** The base class for all binary outputs i.e. outputs that can only be switched on/off.
 *
 * This interface class provides one method you need to override in order to create a binary output
 * component yourself: write_value(). This method will be called for you by the MQTT Component through
 * enable()/disable() to indicate that a new value should be written to hardware.
 *
 * Note that this class also allows the user to invert any state, but you don't need to worry about that
 * because the value will already be inverted (if specified by the user) within set_state_(). So write_state
 * will always receive the correctly inverted state.
 *
 * Additionally, this class provides high power mode capabilities using PowerSupplyComponent. Every time
 * the output is enabled (independent of inversion!), the power supply will automatically be turned on.
 */
class BinaryOutput {
 public:
  /// Set the inversion state of this binary output.
  void set_inverted(bool inverted);

  /** Use this to connect up a power supply to this output.
   *
   * Whenever this output is enabled, the power supply will automatically be turned on.
   *
   * @param power_supply The PowerSupplyComponent, set this to nullptr to disable the power supply.
   */
  void set_power_supply(PowerSupplyComponent *power_supply);

  /// Enable this binary output.
  virtual void turn_on();

  /// Disable this binary output.
  virtual void turn_off();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Return whether this binary output is inverted.
  bool is_inverted() const;

  /// Return the power supply assigned to this binary output.
  PowerSupplyComponent *get_power_supply() const;

  template<typename... Ts> TurnOffAction<Ts...> *make_turn_off_action();
  template<typename... Ts> TurnOnAction<Ts...> *make_turn_on_action();

 protected:
  virtual void write_state(bool state) = 0;

  bool inverted_{false};
  PowerSupplyComponent *power_supply_{nullptr};
  bool has_requested_high_power_{false};
};

template<typename... Ts> class TurnOffAction : public Action<Ts...> {
 public:
  TurnOffAction(BinaryOutput *output);

  void play(Ts... x) override;

 protected:
  BinaryOutput *output_;
};
template<typename... Ts> class TurnOnAction : public Action<Ts...> {
 public:
  TurnOnAction(BinaryOutput *output);

  void play(Ts... x) override;

 protected:
  BinaryOutput *output_;
};

template<typename... Ts> TurnOffAction<Ts...>::TurnOffAction(BinaryOutput *output) : output_(output) {}
template<typename... Ts> void TurnOffAction<Ts...>::play(Ts... x) {
  this->output_->turn_off();
  this->play_next(x...);
}
template<typename... Ts> TurnOnAction<Ts...>::TurnOnAction(BinaryOutput *output) : output_(output) {}
template<typename... Ts> void TurnOnAction<Ts...>::play(Ts... x) {
  this->output_->turn_on();
  this->play_next(x...);
}
template<typename... Ts> TurnOffAction<Ts...> *BinaryOutput::make_turn_off_action() {
  return new TurnOffAction<Ts...>(this);
}
template<typename... Ts> TurnOnAction<Ts...> *BinaryOutput::make_turn_on_action() {
  return new TurnOnAction<Ts...>(this);
}

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_OUTPUT

#endif  // ESPHOME_OUTPUT_BINARY_OUTPUT_H
