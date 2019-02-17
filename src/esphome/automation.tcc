#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

template<typename T>
bool AndCondition<T>::check(T x) {
  for (auto *condition : this->conditions_) {
    if (!condition->check(x))
      return false;
  }

  return true;
}

template<typename T>
AndCondition<T>::AndCondition(const std::vector<Condition<T> *> &conditions)
: conditions_(conditions) {

}

template<typename T>
bool OrCondition<T>::check(T x) {
  for (auto *condition : this->conditions_) {
    if (condition->check(x))
      return true;
  }

  return false;
}

template<typename T>
OrCondition<T>::OrCondition(const std::vector<Condition<T> *> &conditions)
: conditions_(conditions) {

}
template<typename T>
void Trigger<T>::set_parent(Automation<T> *parent) {
  this->parent_ = parent;
}

template<typename T>
void Trigger<T>::trigger(T x) {
  if (this->parent_ == nullptr)
    return;
  this->parent_->process_trigger_(x);
}
template<typename T>
void Trigger<T>::stop() {
  if (this->parent_ == nullptr)
    return;
  this->parent_->stop();
}

template<typename T>
void Action<T>::play_next(T x) {
  if (this->next_ != nullptr) {
    this->next_->play(x);
  }
}
template<typename T>
void Action<T>::stop() {
  this->stop_next();
}
template<typename T>
void Action<T>::stop_next() {
  if (this->next_ != nullptr) {
    this->next_->stop();
  }
}

template<typename T>
DelayAction<T>::DelayAction() = default;

template<typename T>
void DelayAction<T>::play(T x) {
  this->set_timeout(this->delay_.value(x), [this, x](){
    this->play_next(x);
  });
}
template<typename T>
float DelayAction<T>::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
template<typename T>
void DelayAction<T>::stop() {
  this->cancel_timeout("");
  this->stop_next();
}

template<typename T>
Condition<T> *Automation<T>::add_condition(Condition<T> *condition) {
  this->conditions_.push_back(condition);
  return condition;
}
template<typename T>
void Automation<T>::add_conditions(const std::vector<Condition<T> *> &conditions) {
for (auto *condition : conditions) {
this->add_condition(condition);
}
}
template<typename T>
Automation<T>::Automation(Trigger<T> *trigger) : trigger_(trigger) {
  this->trigger_->set_parent(this);
}
template<typename T>
Action<T> *Automation<T>::add_action(Action<T> *action) {
  this->actions_.add_action(action);
}
template<typename T>
void Automation<T>::add_actions(const std::vector<Action<T> *> &actions) {
this->actions_.add_actions(actions);
}
template<typename T>
void Automation<T>::process_trigger_(T x) {
  for (auto *condition : this->conditions_) {
    if (!condition->check(x))
      return;
  }

  this->actions_.play(x);
}
template<typename T>
void Automation<T>::stop() {
  this->actions_.stop();
}
template<typename T>
LambdaCondition<T>::LambdaCondition(std::function<bool(T)> &&f)
    : f_(std::move(f)) {

}
template<typename T>
bool LambdaCondition<T>::check(T x) {
  return this->f_(x);
}

template<typename T>
LambdaAction<T>::LambdaAction(std::function<void(T)> &&f) : f_(std::move(f)) {}
template<typename T>
void LambdaAction<T>::play(T x) {
  this->f_(x);
  this->play_next(x);
}

template<typename T>
Action<T> *ActionList<T>::add_action(Action<T> *action) {
  if (this->actions_end_ == nullptr) {
    this->actions_begin_ = action;
  } else {
    this->actions_end_->next_ = action;
  }
  return this->actions_end_ = action;
}
template<typename T>
void ActionList<T>::add_actions(const std::vector<Action<T> *> &actions) {
for (auto *action : actions) {
this->add_action(action);
}
}
template<typename T>
void ActionList<T>::play(T x) {
  if (this->actions_begin_ != nullptr)
    this->actions_begin_->play(x);
}
template<typename T>
void ActionList<T>::stop() {
  if (this->actions_begin_ != nullptr)
    this->actions_begin_->stop();
}
template<typename T>
bool ActionList<T>::empty() const {
  return this->actions_begin_ == nullptr;
}
template<typename T>
IfAction<T>::IfAction(const std::vector<Condition<T> *> conditions) : conditions_(conditions) {

}
template<typename T>
void IfAction<T>::play(T x) {
  bool res = true;
  for (auto *condition : this->conditions_) {
    if (!condition->check(x)) {
      res = false;
      break;
    }
  }
  if (res) {
    if (this->then_.empty()) {
      this->play_next(x);
    } else {
      this->then_.play(x);
    }
  } else {
    if (this->else_.empty()) {
      this->play_next(x);
    } else {
      this->else_.play(x);
    }
  }
}
template<typename T>
void IfAction<T>::add_then(const std::vector<Action<T> *> &actions) {
  this->then_.add_actions(actions);
  this->then_.add_action(new LambdaAction<T>([this](T x) {
    this->play_next(x);
  }));
}
template<typename T>
void IfAction<T>::add_else(const std::vector<Action<T> *> &actions) {
  this->else_.add_actions(actions);
  this->else_.add_action(new LambdaAction<T>([this](T x) {
    this->play_next(x);
  }));
}
template<typename T>
void IfAction<T>::stop() {
  this->then_.stop();
  this->else_.stop();
  this->stop_next();
}

template<typename T>
void UpdateComponentAction<T>::play(T x) {
  this->component_->update();
  this->play_next(x);
}

template<typename T>
UpdateComponentAction<T>::UpdateComponentAction(PollingComponent *component)
    : component_(component) {

}

template<typename T>
ScriptExecuteAction<T>::ScriptExecuteAction(Script *script)
    : script_(script) {

}

template<typename T>
void ScriptExecuteAction<T>::play(T x) {
  this->script_->trigger();
  this->play_next(x);
}

template<typename T>
ScriptExecuteAction<T> *Script::make_execute_action() {
  return new ScriptExecuteAction<T>(this);
}

template<typename T>
ScriptStopAction<T>::ScriptStopAction(Script *script)
    : script_(script) {

}

template<typename T>
void ScriptStopAction<T>::play(T x) {
  this->script_->stop();
  this->play_next(x);
}

template<typename T>
ScriptStopAction<T> *Script::make_stop_action() {
  return new ScriptStopAction<T>(this);
}

template<typename T>
GlobalVariableComponent<T>::GlobalVariableComponent() {

}
template<typename T>
GlobalVariableComponent<T>::GlobalVariableComponent(T initial_value)
    : value_(initial_value) {

}
template<typename T>
T &GlobalVariableComponent<T>::value() {
  return this->value_;
}
template<typename T>
void GlobalVariableComponent<T>::setup() {
  if (this->restore_value_) {
    this->rtc_ = global_preferences.make_preference<T>(1944399030U ^ this->name_hash_);
    this->rtc_.load(&this->value_);
  }
  memcpy(&this->prev_value_, &this->value_, sizeof(T));
}
template<typename T>
float GlobalVariableComponent<T>::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
template<typename T>
void GlobalVariableComponent<T>::loop() {
  if (this->restore_value_) {
    int diff = memcmp(&this->value_, &this->prev_value_, sizeof(T));
    if (diff != 0) {
      this->rtc_.save(&this->value_);
      memcpy(&this->prev_value_, &this->value_, sizeof(T));
    }
  }
}
template<typename T>
void GlobalVariableComponent<T>::set_restore_value(uint32_t name_hash) {
  this->restore_value_ = true;
  this->name_hash_ = name_hash;
}
template<typename T>
WhileAction<T>::WhileAction(const std::vector<Condition<T> *> &conditions) : conditions_(conditions) {

}
template<typename T>
void WhileAction<T>::add_then(const std::vector<Action<T> *> &actions) {
  this->then_.add_actions(actions);
  this->then_.add_action(new LambdaAction<T>([this](T x) {
    this->is_running_ = false;
    this->play(x);
  }));
}
template<typename T>
void WhileAction<T>::play(T x) {
  if (this->is_running_)
    return;

  for (auto *condition : this->conditions_) {
    if (!condition->check(x)) {
      this->play_next(x);
      return;
    }
  }
  this->is_running_ = true;
  this->then_.play(x);
}
template<typename T>
void WhileAction<T>::stop() {
  this->then_.stop();
  this->is_running_ = false;
  this->stop_next();
}


template<typename T>
WaitUntilAction<T>::WaitUntilAction(const std::vector<Condition<T> *> &conditions)
  : conditions_(conditions) {

}
template<typename T>
void WaitUntilAction<T>::play(T x) {
  this->var_ = x;
  this->triggered_ = true;
  this->loop();
}
template<typename T>
void WaitUntilAction<T>::stop() {
  this->triggered_ = false;
  this->stop_next();
}
template<typename T>
void WaitUntilAction<T>::loop() {
  if (!this->triggered_)
    return;

  for (auto *condition : this->conditions_) {
    if (!condition->check(this->var_)) {
      return;
    }
  }

  this->triggered_ = false;
  this->play_next(this->var_);
}
template<typename T>
float WaitUntilAction<T>::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

ESPHOME_NAMESPACE_END
