//
//  automation.h
//  esphomelib
//
//  Created by Otto Winter on 18.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_AUTOMATION_H
#define ESPHOMELIB_AUTOMATION_H

#include <vector>
#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

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
class Trigger {
 public:
  void add_on_trigger_callback(std::function<void(T)> &&f);
  void trigger(T x);
 protected:
  CallbackManager<void(T)> on_trigger_;
};

template<>
class Trigger<NoArg> {
 public:
  void add_on_trigger_callback(std::function<void(NoArg)> &&f);
  void trigger();
 protected:
  CallbackManager<void(NoArg)> on_trigger_;
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

template<typename T>
class ActionList;

template<typename T>
class Action {
 public:
  virtual void play(T x) = 0;
  void play_next(T x);
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

  void play(T x) override;
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
class ActionList {
 public:
  Action<T> *add_action(Action<T> *action);
  void add_actions(const std::vector<Action<T> *> &actions);
  void play(T x);

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

 protected:
  void process_trigger_(T x);

  Trigger<T> *trigger_;
  std::vector<Condition<T> *> conditions_;
  ActionList<T> actions_;
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

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_AUTOMATION_H
