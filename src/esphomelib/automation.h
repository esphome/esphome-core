#ifndef ESPHOMELIB_AUTOMATION_H
#define ESPHOMELIB_AUTOMATION_H

#include <vector>
#include "esphomelib/espmath.h"
#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"
#include "esphomelib/esppreferences.h"

ESPHOMELIB_NAMESPACE_BEGIN

using NoArg = bool;

template<typename T>
class Condition {
 public:
  virtual bool check(T x) = 0;
};

template<typename T>
class AndCondition : public Condition<T> {
 public:
  explicit AndCondition(const std::vector<Condition<T> *> &conditions);
  bool check(T x) override;
 protected:
  std::vector<Condition<T> *> conditions_;
};

template<typename T>
class OrCondition : public Condition<T> {
 public:
  explicit OrCondition(const std::vector<Condition<T> *> &conditions);
  bool check(T x) override;
 protected:
  std::vector<Condition<T> *> conditions_;
};

template<typename T>
class LambdaCondition : public Condition<T> {
 public:
  explicit LambdaCondition(std::function<bool(T)> &&f);
  bool check(T x) override;
 protected:
  std::function<bool(T)> f_;
};

class RangeCondition : public Condition<float> {
 public:
  explicit RangeCondition();
  bool check(float x) override;

  void set_min(std::function<float(float)> &&min);
  void set_min(float min);
  void set_max(std::function<float(float)> &&max);
  void set_max(float max);

 protected:
  TemplatableValue<float, float> min_{NAN};
  TemplatableValue<float, float> max_{NAN};
};

template<typename T>
class Automation;

template<typename T>
class Trigger {
 public:
  void trigger(T x);
  void set_parent(Automation<T> *parent);
 protected:
  Automation<T> *parent_;
};

template<>
class Trigger<NoArg> {
 public:
  void trigger();
  void trigger(bool arg);
  void set_parent(Automation<NoArg> *parent);
 protected:
  Automation<NoArg> *parent_;
};

class StartupTrigger : public Trigger<NoArg>, public Component {
 public:
  explicit StartupTrigger(float setup_priority = setup_priority::LATE);
  void setup() override;
  float get_setup_priority() const override;

 protected:
  float setup_priority_;
};

class ShutdownTrigger : public Trigger<const char *> {
 public:
  ShutdownTrigger();
};

class LoopTrigger : public Trigger<NoArg>, public Component {
 public:
  void loop() override;
  float get_setup_priority() const override;
};

class IntervalTrigger : public Trigger<NoArg>, public PollingComponent {
 public:
  void loop() override;
  float get_setup_priority() const override;
};

template<typename T>
class ScriptExecuteAction;
template<typename T>
class ScriptStopAction;

template<typename T>
class ScriptStopAction;

class Script : public Trigger<NoArg> {
 public:
  void execute();

  void stop();

  template<typename T>
  ScriptExecuteAction<T> *make_execute_action();

  template<typename T>
  ScriptStopAction<T> *make_stop_action();
};

template<typename T>
class ActionList;

template<typename T>
class Action {
 public:
  virtual void play(T x) = 0;
  void play_next(T x);
  virtual void stop();
  void stop_next();
 protected:
  friend ActionList<T>;

  Action<T> *next_ = nullptr;
};

template<typename T>
class DelayAction : public Action<T>, public Component {
 public:
  explicit DelayAction();

  void set_delay(std::function<uint32_t(T)> &&delay);
  void set_delay(uint32_t delay);
  void stop() override;

  void play(T x) override;
  float get_setup_priority() const override;
 protected:
  TemplatableValue<uint32_t, T> delay_{0};
};

template<typename T>
class LambdaAction : public Action<T> {
 public:
  explicit LambdaAction(std::function<void(T)> &&f);
  void play(T x) override;
 protected:
  std::function<void(T)> f_;
};

template<typename T>
class IfAction : public Action<T> {
 public:
  explicit IfAction(std::vector<Condition<T> *> conditions);

  void add_then(const std::vector<Action<T> *> &actions);

  void add_else(const std::vector<Action<T> *> &actions);

  void play(T x) override;

  void stop() override;

 protected:
  std::vector<Condition<T> *> conditions_;
  ActionList<T> then_;
  ActionList<T> else_;
};

template<typename T>
class WhileAction : public Action<T> {
 public:
  WhileAction(const std::vector<Condition<T> *> &conditions);

  void add_then(const std::vector<Action<T> *> &actions);

  void play(T x) override;

  void stop() override;

 protected:
  std::vector<Condition<T> *> conditions_;
  ActionList<T> then_;
  bool is_running_{false};
};

template<typename T>
class UpdateComponentAction : public Action<T> {
 public:
  UpdateComponentAction(PollingComponent *component);
  void play(T x) override;
 protected:
  PollingComponent *component_;
};

template<typename T>
class ScriptExecuteAction : public Action<T> {
 public:
  ScriptExecuteAction(Script *script);

  void play(T x) override;
 protected:
  Script *script_;
};

template<typename T>
class ScriptStopAction : public Action<T> {
 public:
  ScriptStopAction(Script *script);

  void play(T x) override;
 protected:
  Script *script_;
};

template<typename T>
class ActionList {
 public:
  Action<T> *add_action(Action<T> *action);
  void add_actions(const std::vector<Action<T> *> &actions);
  void play(T x);
  void stop();
  bool empty() const;

 protected:
  Action<T> *actions_begin_{nullptr};
  Action<T> *actions_end_{nullptr};
};

template<typename T>
class Automation {
 public:
  explicit Automation(Trigger<T> *trigger);

  Condition<T> *add_condition(Condition<T> *condition);
  void add_conditions(const std::vector<Condition<T> *> &conditions);

  Action<T> *add_action(Action<T> *action);
  void add_actions(const std::vector<Action<T> *> &actions);

  void process_trigger_(T x);

  void stop();

 protected:
  Trigger<T> *trigger_;
  std::vector<Condition<T> *> conditions_;
  ActionList<T> actions_;
};

template<typename T>
class GlobalVariableComponent : public Component {
 public:
  explicit GlobalVariableComponent();
  explicit GlobalVariableComponent(T initial_value);

  T &value();

  void setup() override;

  float get_setup_priority() const override;

  void loop() override;

  void set_restore_value(uint32_t name_hash);

 protected:
  T value_{};
  T prev_value_{};
  bool restore_value_{false};
  uint32_t name_hash_{};
  ESPPreferenceObject rtc_;
};

// =============== TEMPLATE DEFINITIONS ===============

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
  this->parent_->process_trigger_(x);
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
void DelayAction<T>::set_delay(std::function<uint32_t(T)> &&delay) {
  this->delay_ = std::move(delay);
}
template<typename T>
void DelayAction<T>::set_delay(uint32_t delay) {
  this->delay_ = delay;
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
  this->stop_next();
}

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_AUTOMATION_H
