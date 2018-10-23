#include "esphomelib/automation.h"

ESPHOMELIB_NAMESPACE_BEGIN

void Trigger<NoArg>::add_on_trigger_callback(std::function<void(NoArg)> &&f) {
  this->on_trigger_.add(std::move(f));
}
void Trigger<NoArg>::trigger() {
  this->on_trigger_.call(false);
}

void StartupTrigger::setup() {
  this->trigger();
}
float StartupTrigger::get_setup_priority() const {
  // Run after everything is set up
  return this->setup_priority_;
}
StartupTrigger::StartupTrigger(float setup_priority)
    : setup_priority_(setup_priority) {

}

ShutdownTrigger::ShutdownTrigger() {
  add_shutdown_hook([this](const char *cause){
    this->trigger(cause);
  });
}

void LoopTrigger::loop() {
  this->trigger();
}

RangeCondition::RangeCondition() = default;

bool RangeCondition::check(float x) {
  float min = this->min_.value(x);
  float max = this->max_.value(x);
  if (isnan(min)) {
    return x >= max;
  } else if (isnan(max)) {
    return x >= min;
  } else {
    return min <= x && x <= max;
  }
}
void RangeCondition::set_min(std::function<float(float)> &&min) {
  this->min_ = std::move(min);
}
void RangeCondition::set_min(float min) {
  this->min_ = min;
}
void RangeCondition::set_max(std::function<float(float)> &&max) {
  this->max_ = std::move(max);
}
void RangeCondition::set_max(float max) {
  this->max_ = max;
}

void Script::execute() {
  this->trigger();
}

ESPHOMELIB_NAMESPACE_END

