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
  COVER_COMMAND_POSITION,
  COVER_COMMAND_TILT,
};

using position_value_t = float;
using tilt_value_t = float;

template<typename... Ts> class OpenAction;
template<typename... Ts> class CloseAction;
template<typename... Ts> class StopAction;
template<typename... Ts> class PositionAction;
template<typename... Ts> class TiltAction;
template<typename... Ts> class CoverPublishAction;
template<typename... Ts> class CoverPublishPositionAction;
template<typename... Ts> class CoverPublishTiltAction;

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

  position_value_t position_value;
  tilt_value_t tilt_value;
  void open();
  void close();
  void stop();
  void set_position(position_value_t value);
  void set_tilt(position_value_t value);

  void add_on_publish_state_callback(std::function<void(CoverState)> &&f);

  void publish_state(CoverState state);

  void add_on_publish_position_callback(std::function<void(position_value_t)> &&p);

  void publish_position(position_value_t position_value);

  void add_on_publish_tilt_callback(std::function<void(tilt_value_t)> &&t);

  void publish_tilt(tilt_value_t tilt_value);

  template<typename... Ts> OpenAction<Ts...> *make_open_action();
  template<typename... Ts> CloseAction<Ts...> *make_close_action();
  template<typename... Ts> StopAction<Ts...> *make_stop_action();
  template<typename... Ts> PositionAction<Ts...> *make_position_action();
  template<typename... Ts> TiltAction<Ts...> *make_tilt_action();
  template<typename... Ts> CoverPublishAction<Ts...> *make_cover_publish_action();
  template<typename... Ts> CoverPublishPositionAction<Ts...> *make_cover_publish_position_action();
  template<typename... Ts> CoverPublishTiltAction<Ts...> *make_cover_publish_tilt_action();

  /** Return whether this cover is optimistic - i.e. if both the OPEN/CLOSE actions should be displayed in
   * Home Assistant because the real state is unknown.
   *
   * Defaults to false.
   */
  virtual bool assumed_state();
  void set_optimistic(bool optimistic);
  void set_position_closed(float position_closed);
  void set_position_open(float position_open);
  void set_tilt_min(float tilt_min);
  void set_tilt_max(float tilt_max);
  void set_tilt_closed_value(float tilt_closed_value);
  void set_tilt_opened_value(float tilt_opened_value);
  void set_tilt_invert_state(bool tilt_invert_state);

  bool optimistic();
  float position_closed();
  float position_open();
  float tilt_min();
  float tilt_max();
  float tilt_closed_value();
  float tilt_opened_value();
  bool tilt_invert_state();

  CoverState state{COVER_OPEN};

  bool has_state() const;

#ifdef USE_MQTT_COVER
  MQTTCoverComponent *get_mqtt() const;
  void set_mqtt(MQTTCoverComponent *mqtt);
#endif

 protected:
  virtual void write_command(CoverCommand command) = 0;
  virtual void write_command(CoverCommand command, float value) = 0;

  float position_closed_{0.0};
  float position_open_{100.0};
  float tilt_min_{0.0};
  float tilt_max_{100.0};
  float tilt_closed_value_{0.0};
  float tilt_opened_value_{100.0};
  bool tilt_invert_state_{false};
  bool optimistic_{false};

  uint32_t hash_base() override;

  CallbackManager<void(CoverState)> state_callback_{};
  Deduplicator<CoverState> dedup_;
  CallbackManager<void(position_value_t)> position_callback_{};
  CallbackManager<void(tilt_value_t)> tilt_callback_{};

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

template<typename... Ts> class PositionAction : public Action<Ts...> {
 public:
  explicit PositionAction(Cover *cover, float value);

  void set_position(float value) { this->position_ = value; }

  void play(Ts... x) override;

 protected:
  Cover *cover_;
};

template<typename... Ts> class TiltAction : public Action<Ts...> {
 public:
  explicit TiltAction(Cover *cover, float value);

  void set_tilt(float value) { this->tilt_ = value; }

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

template<typename... Ts> class CoverPublishPositionAction : public Action<Ts...> {
 public:
  CoverPublishPositionAction(Cover *cover);
  template<typename V> void set_position(V value) { this->position_ = value; }
  void play(Ts... x) override;

 protected:
  Cover *cover_;
  TemplatableValue<position_value_t, Ts...> position_;
};

template<typename... Ts> class CoverPublishTiltAction : public Action<Ts...> {
 public:
  CoverPublishTiltAction(Cover *cover);
  template<typename V> void set_tilt(V value) { this->tilt_ = value; }
  void play(Ts... x) override;

 protected:
  Cover *cover_;
  TemplatableValue<tilt_value_t, Ts...> tilt_;
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

template<typename... Ts> PositionAction<Ts...>::PositionAction(Cover *cover, position_value_t value) : cover_(cover) {}

template<typename... Ts> void PositionAction<Ts...>::play(Ts... x) {
  this->cover_->set_position(x...);
  this->play_next(x...);
}

template<typename... Ts> TiltAction<Ts...>::TiltAction(Cover *cover, tilt_value_t value) : cover_(cover) {}

template<typename... Ts> void TiltAction<Ts...>::play(Ts... x) {
  this->cover_->set_tilt(x...);
  this->play_next(x...);
}

template<typename... Ts> OpenAction<Ts...> *Cover::make_open_action() { return new OpenAction<Ts...>(this); }
template<typename... Ts> CloseAction<Ts...> *Cover::make_close_action() { return new CloseAction<Ts...>(this); }
template<typename... Ts> StopAction<Ts...> *Cover::make_stop_action() { return new StopAction<Ts...>(this); }

template<typename... Ts> PositionAction<Ts...> *Cover::make_position_action() { return new PositionAction<Ts...>(this); }
template<typename... Ts> TiltAction<Ts...> *Cover::make_tilt_action() { return new TiltAction<Ts...>(this); }

template<typename... Ts> CoverPublishAction<Ts...>::CoverPublishAction(Cover *cover) : cover_(cover) {}
template<typename... Ts> void CoverPublishAction<Ts...>::play(Ts... x) {
  auto val = this->state_.value(x...);
  this->cover_->publish_state(val);
  this->play_next(x...);
}
template<typename... Ts> CoverPublishAction<Ts...> *Cover::make_cover_publish_action() {
  return new CoverPublishAction<Ts...>(this);
}
template<typename... Ts> CoverPublishPositionAction<Ts...>::CoverPublishPositionAction(Cover *cover) : cover_(cover) {}
template<typename... Ts> void CoverPublishPositionAction<Ts...>::play(Ts... x) {
  auto val = this->position_.value(x...);
  this->cover_->publish_position(val);
  this->play_next(x...);
}
template<typename... Ts> CoverPublishPositionAction<Ts...> *Cover::make_cover_publish_position_action() {
  return new CoverPublishPositionAction<Ts...>(this);
}
template<typename... Ts> CoverPublishTiltAction<Ts...>::CoverPublishTiltAction(Cover *cover) : cover_(cover) {}
template<typename... Ts> void CoverPublishTiltAction<Ts...>::play(Ts... x) {
  auto val = this->tilt_.value(x...);
  this->cover_->publish_tilt(val);
  this->play_next(x...);
}
template<typename... Ts> CoverPublishTiltAction<Ts...> *Cover::make_cover_publish_tilt_action() {
  return new CoverPublishTiltAction<Ts...>(this);
}

}  // namespace cover

ESPHOME_NAMESPACE_END

#include "esphome/cover/mqtt_cover_component.h"

#endif  // USE_COVER

#endif  // ESPHOME_COVER_COVER_H
