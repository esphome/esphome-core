#ifndef ESPHOME_COVER_COVER_H
#define ESPHOME_COVER_COVER_H

#include "esphome/defines.h"

#ifdef USE_COVER

#include "esphome/component.h"
#include "esphome/helpers.h"
#include "esphome/automation.h"
#include "esphome/cover/cover_traits.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

enum CoverState : uint8_t {
  COVER_OPEN = 0,
  COVER_CLOSED,
};

const char *cover_state_to_str(CoverState state);

enum CoverCommand : uint8_t {
  COVER_COMMAND_OPEN = 0,
  COVER_COMMAND_CLOSE,
  COVER_COMMAND_STOP,
};

const char *cover_command_to_str(CoverCommand command);

template<typename... Ts> class OpenAction;
template<typename... Ts> class CloseAction;
template<typename... Ts> class StopAction;
template<typename... Ts> class CoverPublishAction;

#define LOG_COVER(prefix, type, obj) \
  if (obj != nullptr) { \
    ESP_LOGCONFIG(TAG, prefix type " '%s'", obj->get_name().c_str()); \
    auto traits_ = obj->get_traits(); \
    if (traits_.get_is_assumed_state()) { \
      ESP_LOGCONFIG(TAG, prefix "  Assumed State: YES"); \
    } \
    if (!traits_.get_device_class().empty()) { \
      ESP_LOGCONFIG(TAG, prefix "  Device Class: '%s'", traits_.get_device_class().c_str()); \
    } \
  }

#ifdef USE_MQTT_COVER
class MQTTCoverComponent;
#endif

class Cover;

class CoverCall {
 public:
  CoverCall(Cover *parent);

  CoverCall &set_command(CoverCommand command);
  CoverCall &set_command(const char *command);
  CoverCall &set_command_open();
  CoverCall &set_command_close();
  CoverCall &set_command_stop();
  CoverCall &set_position(float position);
  CoverCall &set_tilt(float tilt);

  void perform() const;

  const optional<CoverCommand> &get_command() const;
  const optional<float> &get_position() const;
  const optional<float> &get_tilt() const;

 protected:
  Cover *parent_;
  optional<CoverCommand> command_{};
  optional<float> position_{};
  optional<float> tilt_{};
};

struct CoverRestoreState {
  CoverState state;
  float position;
  float tilt;
} __attribute__((packed));

class Cover : public Nameable {
 public:
  explicit Cover(const std::string &name);

  CoverState state{COVER_OPEN};
  float position{0.0f};
  float tilt{0.0f};

  CoverCall make_call();
  void open();
  void close();
  void stop();

  void add_on_state_callback(std::function<void()> &&f);

  template<typename... Ts> OpenAction<Ts...> *make_open_action();
  template<typename... Ts> CloseAction<Ts...> *make_close_action();
  template<typename... Ts> StopAction<Ts...> *make_stop_action();
  template<typename... Ts> CoverPublishAction<Ts...> *make_cover_publish_action();

  void publish_state();

#ifdef USE_MQTT_COVER
  MQTTCoverComponent *get_mqtt() const;
  void set_mqtt(MQTTCoverComponent *mqtt);
#endif

  CoverTraits get_traits();
  void set_device_class(const std::string &device_class);

 protected:
  friend CoverCall;

  virtual void control(const CoverCall &call) = 0;

  virtual CoverTraits traits() = 0;

  optional<CoverRestoreState> restore_state_();

  uint32_t hash_base() override;

  CallbackManager<void()> state_callback_{};
  std::string device_class_override_{};

#ifdef USE_MQTT_COVER
  MQTTCoverComponent *mqtt_{nullptr};
#endif
  ESPPreferenceObject rtc_;
};

template<typename... Ts> class OpenAction : public Action<Ts...> {
 public:
  explicit OpenAction(Cover *cover);

  void play(Ts... x) override;

 protected:
  Cover *cover_;
};

template<typename... Ts> class CloseAction : public Action<Ts...> {
 public:
  explicit CloseAction(Cover *cover);

  void play(Ts... x) override;

 protected:
  Cover *cover_;
};

template<typename... Ts> class StopAction : public Action<Ts...> {
 public:
  explicit StopAction(Cover *cover);

  void play(Ts... x) override;

 protected:
  Cover *cover_;
};

template<typename... Ts> class CoverPublishAction : public Action<Ts...> {
 public:
  CoverPublishAction(Cover *cover);
  template<typename V> void set_state(V value) { this->state_ = value; }
  void play(Ts... x) override;

 protected:
  Cover *cover_;
  TemplatableValue<CoverState, Ts...> state_;
};

// =============== TEMPLATE DEFINITIONS ===============

template<typename... Ts> OpenAction<Ts...>::OpenAction(Cover *cover) : cover_(cover) {}
template<typename... Ts> void OpenAction<Ts...>::play(Ts... x) {
  this->cover_->open();
  this->play_next(x...);
}

template<typename... Ts> CloseAction<Ts...>::CloseAction(Cover *cover) : cover_(cover) {}
template<typename... Ts> void CloseAction<Ts...>::play(Ts... x) {
  this->cover_->close();
  this->play_next(x...);
}

template<typename... Ts> StopAction<Ts...>::StopAction(Cover *cover) : cover_(cover) {}

template<typename... Ts> void StopAction<Ts...>::play(Ts... x) {
  this->cover_->stop();
  this->play_next(x...);
}

template<typename... Ts> OpenAction<Ts...> *Cover::make_open_action() { return new OpenAction<Ts...>(this); }

template<typename... Ts> CloseAction<Ts...> *Cover::make_close_action() { return new CloseAction<Ts...>(this); }
template<typename... Ts> StopAction<Ts...> *Cover::make_stop_action() { return new StopAction<Ts...>(this); }

template<typename... Ts> CoverPublishAction<Ts...>::CoverPublishAction(Cover *cover) : cover_(cover) {}
template<typename... Ts> void CoverPublishAction<Ts...>::play(Ts... x) {
  auto val = this->state_.value(x...);
  this->cover_->publish_state(val);
  this->play_next(x...);
}
template<typename... Ts> CoverPublishAction<Ts...> *Cover::make_cover_publish_action() {
  return new CoverPublishAction<Ts...>(this);
}

}  // namespace cover

ESPHOME_NAMESPACE_END

#include "esphome/cover/mqtt_cover_component.h"

#endif  // USE_COVER

#endif  // ESPHOME_COVER_COVER_H
