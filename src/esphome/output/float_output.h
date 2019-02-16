#ifndef ESPHOME_OUTPUT_FLOAT_OUTPUT_H
#define ESPHOME_OUTPUT_FLOAT_OUTPUT_H

#include "esphome/defines.h"

#ifdef USE_OUTPUT

#include "esphome/output/binary_output.h"
#include "esphome/component.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

template<typename T>
class SetLevelAction;

#define LOG_FLOAT_OUTPUT(this) \
  LOG_BINARY_OUTPUT(this) \
  if (this->max_power_ != 1.0f) { ESP_LOGCONFIG(TAG, "  Max Power: %.1f%%", this->max_power_ * 100.0f); }

/** Base class for all output components that can output a variable level, like PWM.
 *
 * Floating Point Outputs always use output values in the range from 0.0 to 1.0 (inclusive), where 0.0 means off
 * and 1.0 means fully on. While using floating point numbers might make computation slower, it
 * makes using maths much easier and (in theory) supports all possible bit depths.
 *
 * If you want to create a FloatOutput yourself, you essentially just have to override write_state(float).
 * That method will be called for you with inversion and max power already applied. It is
 *
 * This interface is compatible with BinaryOutput (and will automatically convert the binary states to floating
 * point states for you). Additionally, this class provides a way for users to set a maximum power
 * output
 */
class FloatOutput : public BinaryOutput {
 public:
  /** Set the maximum power output of this component.
   *
   * All values are multiplied by this float to get the adjusted value.
   *
   * @param max_power Automatically clamped from 0 to 1.
   */
  void set_max_power(float max_power);

  /// Set the level of this float output, this is called from the front-end.
  void set_level(float state);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  /// Get the maximum power output.
  float get_max_power() const;

  /// Implement BinarySensor's write_enabled; this should never be called.
  void write_state(bool value) override;

  template<typename T>
  SetLevelAction<T> *make_set_level_action();

 protected:
  virtual void write_state(float state) = 0;

  float max_power_{1.0f};
};

template<typename T>
class SetLevelAction : public Action<T> {
 public:
  SetLevelAction(FloatOutput *output);

  template<typename V>
  void set_level(V level) { this->level_ = level; }
  void play(T x) override;

 protected:
  FloatOutput *output_;
  TemplatableValue<float, T> level_;
};

template<typename T>
SetLevelAction<T>::SetLevelAction(FloatOutput *output) : output_(output) {}

template<typename T>
void SetLevelAction<T>::play(T x) {
  this->output_->set_level(this->level_.value(x));
  this->play_next(x);
}

template<typename T>
SetLevelAction<T> *FloatOutput::make_set_level_action() {
  return new SetLevelAction<T>(this);
}

} // namespace output

ESPHOME_NAMESPACE_END

#endif //USE_OUTPUT

#endif //ESPHOME_OUTPUT_FLOAT_OUTPUT_H
