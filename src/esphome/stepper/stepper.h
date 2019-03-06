#ifndef ESPHOME_STEPPER_STEPPER_H
#define ESPHOME_STEPPER_STEPPER_H

#include "esphome/defines.h"

#ifdef USE_STEPPER

#include "esphome/component.h"
#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

namespace stepper {

template<typename... Ts> class SetTargetAction;
template<typename... Ts> class ReportPositionAction;

#define LOG_STEPPER(this) \
  ESP_LOGCONFIG(TAG, "  Acceleration: %.0f steps/s^2", this->acceleration_); \
  ESP_LOGCONFIG(TAG, "  Deceleration: %.0f steps/s^2", this->deceleration_); \
  ESP_LOGCONFIG(TAG, "  Max Speed: %.0f steps/s", this->max_speed_);

class Stepper {
 public:
  void set_target(int32_t steps);
  void report_position(int32_t steps);
  void set_acceleration(float acceleration);
  void set_deceleration(float deceleration);
  void set_max_speed(float max_speed);
  bool has_reached_target();

  template<typename... Ts> SetTargetAction<Ts...> *make_set_target_action();
  template<typename... Ts> ReportPositionAction<Ts...> *make_report_position_action();

  int32_t current_position{0};
  int32_t target_position{0};

 protected:
  void calculate_speed_(uint32_t now);
  int32_t should_step_();

  float acceleration_{1e6f};
  float deceleration_{1e6f};
  float current_speed_{0.0f};
  float max_speed_{1e6f};
  uint32_t last_calculation_{0};
  uint32_t last_step_{0};
};

template<typename... Ts> class SetTargetAction : public Action<Ts...> {
 public:
  explicit SetTargetAction(Stepper *parent);

  template<typename V> void set_target(V target) { this->target_ = target; }

  void play(Ts... x) override;

 protected:
  Stepper *parent_;
  TemplatableValue<int32_t, Ts...> target_;
};

template<typename... Ts> class ReportPositionAction : public Action<Ts...> {
 public:
  explicit ReportPositionAction(Stepper *parent);

  template<typename V> void set_position(V position) { this->position_ = position; }

  void play(Ts... x) override;

 protected:
  Stepper *parent_;
  TemplatableValue<int32_t, Ts...> position_;
};
template<typename... Ts> ReportPositionAction<Ts...>::ReportPositionAction(Stepper *parent) : parent_(parent) {}
template<typename... Ts> void ReportPositionAction<Ts...>::play(Ts... x) {
  this->parent_->report_position(this->position_.value(x...));
  this->play_next(x...);
}

template<typename... Ts> SetTargetAction<Ts...> *Stepper::make_set_target_action() {
  return new SetTargetAction<Ts...>(this);
}
template<typename... Ts> ReportPositionAction<Ts...> *Stepper::make_report_position_action() {
  return new ReportPositionAction<Ts...>(this);
}
template<typename... Ts> SetTargetAction<Ts...>::SetTargetAction(Stepper *parent) : parent_(parent) {}
template<typename... Ts> void SetTargetAction<Ts...>::play(Ts... x) {
  this->parent_->set_target(this->target_.value(x...));
  this->play_next(x...);
}

}  // namespace stepper

ESPHOME_NAMESPACE_END

#endif  // USE_STEPPER

#endif  // ESPHOME_STEPPER_STEPPER_H
