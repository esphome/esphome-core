#include "esphomelib/automation.h"

ESPHOMELIB_NAMESPACE_BEGIN

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
void Trigger<T>::add_on_trigger_callback(std::function<void(T)> &&f) {
  this->on_trigger_.add(std::move(f));
}

template<typename T>
void Trigger<T>::trigger(T x) {
  this->on_trigger_.call(x);
}

template<typename T>
void Action<T>::play_next(T x) {
  if (this->next_ != nullptr)
    this->next_->play(x);
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
void DelayAction<T>::set_delay(std::function<uint32_t(T)> &&delay) {
  this->delay_ = std::move(delay);
}
template<typename T>
void DelayAction<T>::set_delay(uint32_t delay) {
  this->delay_ = delay;
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
  this->trigger_->add_on_trigger_callback([this](T x) {
    this->process_trigger_(x);
  });
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

ESPHOMELIB_NAMESPACE_END
