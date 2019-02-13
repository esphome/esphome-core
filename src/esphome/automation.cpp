#include "esphome/automation.h"
#include "esphome/espmath.h"

ESPHOME_NAMESPACE_BEGIN

void Trigger<NoArg>::trigger() {
  if (this->parent_ == nullptr)
    return;
  this->parent_->process_trigger_(false);
}
void Trigger<NoArg>::trigger(bool arg) {
  if (this->parent_ == nullptr)
    return;
  this->parent_->process_trigger_(arg);
}
void Trigger<NoArg>::stop() {
  if (this->parent_ == nullptr)
    return;
  this->parent_->stop();
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

void IntervalTrigger::update() {
  this->trigger();
}
float IntervalTrigger::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

IntervalTrigger::IntervalTrigger(uint32_t update_interval) : PollingComponent(update_interval) {}

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

void Script::execute() {
  this->trigger();
}
void Script::stop() {
  this->parent_->stop();
}

ESPHOME_NAMESPACE_END

