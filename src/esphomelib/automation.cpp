#include "esphomelib/automation.h"
#include "esphomelib/espmath.h"

ESPHOMELIB_NAMESPACE_BEGIN

void Trigger<NoArg>::trigger() {
  this->parent_->process_trigger_(false);
}
void Trigger<NoArg>::trigger(bool arg) {
  this->parent_->process_trigger_(arg);
}
void Trigger<NoArg>::set_parent(Automation<NoArg> *parent) {
  this->parent_ = parent;
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
float LoopTrigger::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

void IntervalTrigger::loop() {
  this->trigger();
}
float IntervalTrigger::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
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
void Script::stop() {
  this->parent_->stop();
}

ESPHOMELIB_NAMESPACE_END

