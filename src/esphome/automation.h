#ifndef ESPHOME_AUTOMATION_H
#define ESPHOME_AUTOMATION_H

#include <vector>
#include "esphome/espmath.h"
#include "esphome/component.h"
#include "esphome/helpers.h"
#include "esphome/defines.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

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

  template<typename V>
  void set_min(V value) { this->min_ = value; }
  template<typename V>
  void set_max(V value) { this->max_ = value; }

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
  void stop();
 protected:
  Automation<T> *parent_{nullptr};
};

template<>
class Trigger<NoArg> {
 public:
  void trigger();
  void trigger(bool arg);
  void set_parent(Automation<NoArg> *parent);
  void stop();
 protected:
  Automation<NoArg> *parent_{nullptr};
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
  IntervalTrigger(uint32_t update_interval);
  void update() override;
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

  template<typename V>
  void set_delay(V value) { this->delay_ = value; }
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
class WaitUntilAction : public Action<T>, public Component {
 public:
  WaitUntilAction(const std::vector<Condition<T> *> &conditions);

  void play(T x) override;

  void stop() override;

  void loop() override;

  float get_setup_priority() const override;

 protected:
  std::vector<Condition<T> *> conditions_;
  bool triggered_{false};
  T var_{};
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

ESPHOME_NAMESPACE_END

#include "esphome/automation.tcc"

#endif //ESPHOME_AUTOMATION_H
