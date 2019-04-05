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

const extern float COVER_OPEN;
const extern float COVER_CLOSED;

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
    if (!obj->get_device_class().empty()) { \
      ESP_LOGCONFIG(TAG, prefix "  Device Class: '%s'", obj->get_device_class().c_str()); \
    } \
  }

#ifdef USE_MQTT_COVER
class MQTTCoverComponent;
#endif

class Cover;

class CoverCall {
 public:
  CoverCall(Cover *parent);

  CoverCall &set_command(const char *command);
  CoverCall &set_command_open();
  CoverCall &set_command_close();
  CoverCall &set_command_stop();
  CoverCall &set_position(float position);
  CoverCall &set_tilt(float tilt);

  void perform();

  const optional<float> &get_position() const;
  bool get_stop() const { return this->stop_; }
  const optional<float> &get_tilt() const;

 protected:
  void validate_();

  Cover *parent_;
  bool stop_{false};
  optional<float> position_{};
  optional<float> tilt_{};
};

struct CoverRestoreState {
  float position;
  float tilt;

  CoverCall to_call(Cover *cover);
  void apply(Cover *cover);
} __attribute__((packed));

enum CoverOperation : uint8_t {
  COVER_OPERATION_IDLE = 0,
  COVER_OPERATION_IS_OPENING,
  COVER_OPERATION_IS_CLOSING,
};

class Cover : public Nameable {
 public:
  explicit Cover(const std::string &name);
  explicit Cover() : Cover("") {}

  CoverOperation current_operation{COVER_OPERATION_IDLE};
  union {
    float position;
    ESPDEPRECATED("<cover>.state is deprecated, please use .position instead") float state;
  };
  float tilt{COVER_OPEN};

  CoverCall make_call();
  void open();
  void close();
  void stop();

  void add_on_state_callback(std::function<void()> &&f);

  void publish_state();

#ifdef USE_MQTT_COVER
  MQTTCoverComponent *get_mqtt() const;
  void set_mqtt(MQTTCoverComponent *mqtt);
#endif

  virtual CoverTraits get_traits() = 0;
  void set_device_class(const std::string &device_class);
  std::string get_device_class() {
    if (this->device_class_override_.has_value())
      return *this->device_class_override_;
    return this->device_class();
  }

 protected:
  friend CoverCall;

  virtual void control(const CoverCall &call) = 0;
  virtual std::string device_class() { return ""; }

  optional<CoverRestoreState> restore_state_();
  uint32_t hash_base() override;

  CallbackManager<void()> state_callback_{};
  optional<std::string> device_class_override_{};

#ifdef USE_MQTT_COVER
  MQTTCoverComponent *mqtt_{nullptr};
#endif
  ESPPreferenceObject rtc_;
};

}  // namespace cover

ESPHOME_NAMESPACE_END

#include "esphome/cover/mqtt_cover_component.h"
#include "esphome/cover/cover_automation.h"

#endif  // USE_COVER

#endif  // ESPHOME_COVER_COVER_H
