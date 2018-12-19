#ifndef ESPHOMELIB_COVER_COVER_H
#define ESPHOMELIB_COVER_COVER_H

#include "esphomelib/defines.h"

#ifdef USE_COVER

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/automation.h"

ESPHOMELIB_NAMESPACE_BEGIN

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

template<typename T>
class OpenAction;
template<typename T>
class CloseAction;
template<typename T>
class StopAction;

class Cover : public Nameable {
 public:
  explicit Cover(const std::string &name);

  void open();
  void close();
  void stop();

  void add_on_publish_state_callback(std::function<void(CoverState)> &&f);

  void publish_state(CoverState state);

  template<typename T>
  OpenAction<T> *make_open_action();
  template<typename T>
  CloseAction<T> *make_close_action();
  template<typename T>
  StopAction<T> *make_stop_action();

  /** Return whether this cover is optimistic - i.e. if both the OPEN/CLOSE actions should be displayed in
   * Home Assistant because the real state is unknown.
   *
   * Defaults to false.
   */
  virtual bool optimistic();

  CoverState state{COVER_OPEN};

  bool has_state() const;

 protected:
  virtual void write_command(CoverCommand command) = 0;

  uint32_t hash_base_() override;

  bool has_state_{false};
  CallbackManager<void(CoverState)> state_callback_{};
};

template<typename T>
class OpenAction : public Action<T> {
 public:
  explicit OpenAction(Cover *cover);

  void play(T x) override;

 protected:
  Cover *cover_;
};

template<typename T>
class CloseAction : public Action<T> {
 public:
  explicit CloseAction(Cover *cover);

  void play(T x) override;

 protected:
  Cover *cover_;
};

template<typename T>
class StopAction : public Action<T> {
 public:
  explicit StopAction(Cover *cover);

  void play(T x) override;

 protected:
  Cover *cover_;
};

// =============== TEMPLATE DEFINITIONS ===============

template<typename T>
OpenAction<T>::OpenAction(Cover *cover)
    : cover_(cover) {

}
template<typename T>
void OpenAction<T>::play(T x) {
  this->cover_->open();
  this->play_next(x);
}

template<typename T>
CloseAction<T>::CloseAction(Cover *cover)
    : cover_(cover) {

}
template<typename T>
void CloseAction<T>::play(T x) {
  this->cover_->close();
  this->play_next(x);
}

template<typename T>
StopAction<T>::StopAction(Cover *cover)
    : cover_(cover) {

}

template<typename T>
void StopAction<T>::play(T x) {
  this->cover_->stop();
  this->play_next(x);
}

template<typename T>
OpenAction<T> *Cover::make_open_action() {
  return new OpenAction<T>(this);
}

template<typename T>
CloseAction<T> *Cover::make_close_action() {
  return new CloseAction<T>(this);
}
template<typename T>
StopAction<T> *Cover::make_stop_action() {
  return new StopAction<T>(this);
}

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_COVER

#endif //ESPHOMELIB_COVER_COVER_H
