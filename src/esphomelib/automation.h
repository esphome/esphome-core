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

class LoopTrigger : public Trigger<NoArg>, public Component {
 public:
  void loop() override;
};

template<typename T>
class ScriptExecuteAction;

class Script : public Trigger<NoArg> {
 public:
  void execute();

  template<typename T>
  ScriptExecuteAction<T> *make_execute_action();
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
class IfAction : public Action<T> {
 public:
  explicit IfAction(std::vector<Condition<T> *> conditions);

  void add_then(const std::vector<Action<T> *> &actions);

  void add_else(const std::vector<Action<T> *> &actions);

  void play(T x) override;

 protected:
  std::vector<Condition<T> *> conditions_;
  ActionList<T> then_;
  ActionList<T> else_;
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
class ActionList {
 public:
  Action<T> *add_action(Action<T> *action);
  void add_actions(const std::vector<Action<T> *> &actions);
  void play(T x);
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

 protected:
  void process_trigger_(T x);

  Trigger<T> *trigger_;
  std::vector<Condition<T> *> conditions_;
  ActionList<T> actions_;
};

ESPHOMELIB_NAMESPACE_END

#include "esphomelib/automation.tcc"

#endif //ESPHOMELIB_AUTOMATION_H
