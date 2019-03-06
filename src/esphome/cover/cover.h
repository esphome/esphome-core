#ifndef ESPHOME_COVER_COVER_H
#define ESPHOME_COVER_COVER_H

#include "esphome/defines.h"

#ifdef USE_COVER

#include "esphome/component.h"
#include "esphome/helpers.h"
#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

enum CoverState {
  COVER_OPEN = 0,
  COVER_CLOSED,
};

enum CoverCommand {
  COVER_COMMAND_OPEN = 0,
  COVER_COMMAND_CLOSE,
  COVER_COMMAND_STOP,
};

template<typename... Ts> class OpenAction;
template<typename... Ts> class CloseAction;
template<typename... Ts> class StopAction;
template<typename... Ts> class CoverPublishAction;

#define LOG_COVER(prefix, type, obj) \
  if (obj != nullptr) { \
    ESP_LOGCONFIG(TAG, prefix type " '%s'", obj->get_name().c_str()); \
    if (obj->assumed_state()) { \
      ESP_LOGCONFIG(TAG, prefix "  Assumed State: YES"); \
    } \
  }

#ifdef USE_MQTT_COVER
class MQTTCoverComponent;
#endif

class Cover : public Nameable {
 public:
  explicit Cover(const std::string &name);

  void open();
  void close();
  void stop();

  void add_on_publish_state_callback(std::function<void(CoverState)> &&f);

  void publish_state(CoverState state);

  template<typename... Ts> OpenAction<Ts...> *make_open_action();
  template<typename... Ts> CloseAction<Ts...> *make_close_action();
  template<typename... Ts> StopAction<Ts...> *make_stop_action();
  template<typename... Ts> CoverPublishAction<Ts...> *make_cover_publish_action();

  /** Return whether this cover is optimistic - i.e. if both the OPEN/CLOSE actions should be displayed in
   * Home Assistant because the real state is unknown.
   *
   * Defaults to false.
   */
  virtual bool assumed_state();

  CoverState state{COVER_OPEN};

  bool has_state() const;

#ifdef USE_MQTT_COVER
  MQTTCoverComponent *get_mqtt() const;
  void set_mqtt(MQTTCoverComponent *mqtt);
#endif

 protected:
  virtual void write_command(CoverCommand command) = 0;

  uint32_t hash_base() override;

  CallbackManager<void(CoverState)> state_callback_{};
  Deduplicator<CoverState> dedup_;

#ifdef USE_MQTT_COVER
  MQTTCoverComponent *mqtt_{nullptr};
#endif
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
