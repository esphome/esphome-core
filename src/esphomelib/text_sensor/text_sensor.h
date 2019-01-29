#ifndef ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H
#define ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_TEXT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/automation.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class TextSensorStateTrigger;
template<typename T>
class TextSensorPublishAction;

#define LOG_TEXT_SENSOR(prefix, type, obj) \
    if (obj != nullptr) { \
      ESP_LOGCONFIG(TAG, prefix type " '%s'", obj->get_name().c_str()); \
      if (!obj->get_icon().empty()) { \
        ESP_LOGCONFIG(TAG, prefix "  Icon: '%s'", obj->get_icon().c_str()); \
      } \
      if (!obj->unique_id().empty()) { \
        ESP_LOGV(TAG, prefix "  Unique ID: '%s'", obj->unique_id().c_str()); \
      } \
    }

class TextSensor : public Nameable {
 public:
  explicit TextSensor(const std::string &name) : Nameable(name) {}

  void publish_state(std::string state);

  void set_icon(const std::string &icon);

  void add_on_state_callback(std::function<void(std::string)> callback);

  std::string state;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  std::string get_icon();

  virtual std::string icon();

  virtual std::string unique_id();

  TextSensorStateTrigger *make_state_trigger();
  template<typename T>
  TextSensorPublishAction<T> *make_text_sensor_publish_action();

  bool has_state();

 protected:
  uint32_t hash_base_() override;

  CallbackManager<void(std::string)> callback_;
  optional<std::string> icon_;
  bool has_state_{false};
};

class TextSensorStateTrigger : public Trigger<std::string> {
 public:
  explicit TextSensorStateTrigger(TextSensor *parent);
};

template<typename T>
class TextSensorPublishAction : public Action<T> {
 public:
  TextSensorPublishAction(TextSensor *sensor);
  void set_state(std::function<std::string(T)> &&value);
  void set_state(std::string value);
  void play(T x) override;
 protected:
  TextSensor *sensor_;
  TemplatableValue<std::string, T> value_;
};

template<typename T>
TextSensorPublishAction<T>::TextSensorPublishAction(TextSensor *sensor) : sensor_(sensor) {}
template<typename T>
void TextSensorPublishAction<T>::set_state(std::function<std::string(T)> &&value) {
  this->value_ = std::move(value);
}
template<typename T>
void TextSensorPublishAction<T>::set_state(std::string value) {
  this->value_ = value;
}
template<typename T>
void TextSensorPublishAction<T>::play(T x) {
  this->sensor_->publish_state(this->value_.value(x));
  this->play_next(x);
}
template<typename T>
TextSensorPublishAction<T> *TextSensor::make_text_sensor_publish_action() {
  return new TextSensorPublishAction<T>(this);
}

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEXT_SENSOR

#endif //ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H
