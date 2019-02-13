#ifndef ESPHOME_STEPPER_STEPPER_H
#define ESPHOME_STEPPER_STEPPER_H

#include "esphome/defines.h"

#ifdef USE_STEPPER

#include "esphome/component.h"
#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

namespace stepper {

template<typename T>
class SetTargetAction;

template<typename T>
class ReportPositionAction;

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

  template<typename T>
  SetTargetAction<T> *make_set_target_action();
  template<typename T>
  ReportPositionAction<T> *make_report_position_action();

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

template<typename T>
class SetTargetAction : public Action<T> {
 public:
  explicit SetTargetAction(Stepper *parent);

  template<typename V>
  void set_target(V target) { this->target_ = target; }

  void play(T x) override;

 protected:
  Stepper *parent_;
  TemplatableValue<int32_t, T> target_;
};

template<typename T>
class ReportPositionAction : public Action<T> {
 public:
  explicit ReportPositionAction(Stepper *parent);

  template<typename V>
  void set_position(V position) { this->position_ = position; }

  void play(T x) override;

 protected:
  Stepper *parent_;
  TemplatableValue<int32_t, T> position_;
};
template<typename T>
ReportPositionAction<T>::ReportPositionAction(Stepper *parent)
    : parent_(parent) {

}
template<typename T>
void ReportPositionAction<T>::play(T x) {
  this->parent_->report_position(this->position_.value(x));
  this->play_next(x);
}

template<typename T>
SetTargetAction<T> *Stepper::make_set_target_action() {
  return new SetTargetAction<T>(this);
}
template<typename T>
ReportPositionAction<T> *Stepper::make_report_position_action() {
  return new ReportPositionAction<T>(this);
}
template<typename T>
SetTargetAction<T>::SetTargetAction(Stepper *parent)
    : parent_(parent) {

}
template<typename T>
void SetTargetAction<T>::play(T x) {
  this->parent_->set_target(this->target_.value(x));
  this->play_next(x);
}

} // namespace stepper

ESPHOME_NAMESPACE_END

#endif //USE_STEPPER

#endif //ESPHOME_STEPPER_STEPPER_H
