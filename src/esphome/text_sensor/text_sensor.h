#ifndef ESPHOME_TEXT_SENSOR_TEXT_SENSOR_H
#define ESPHOME_TEXT_SENSOR_TEXT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_TEXT_SENSOR

#include "esphome/component.h"
#include "esphome/automation.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class TextSensorStateTrigger;
template<typename... Ts> class TextSensorPublishAction;

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

#ifdef USE_MQTT_TEXT_SENSOR
class MQTTTextSensor;
#endif

class TextSensor : public Nameable {
 public:
  explicit TextSensor();
  explicit TextSensor(const std::string &name);

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
  template<typename... Ts> TextSensorPublishAction<Ts...> *make_text_sensor_publish_action();

  bool has_state();

#ifdef USE_MQTT_TEXT_SENSOR
  MQTTTextSensor *get_mqtt() const;
  void set_mqtt(MQTTTextSensor *mqtt);
#endif

 protected:
  uint32_t hash_base() override;

  CallbackManager<void(std::string)> callback_;
  optional<std::string> icon_;
  bool has_state_{false};
#ifdef USE_MQTT_TEXT_SENSOR
  MQTTTextSensor *mqtt_{nullptr};
#endif
};

class TextSensorStateTrigger : public Trigger<std::string> {
 public:
  explicit TextSensorStateTrigger(TextSensor *parent);
};

template<typename... Ts> class TextSensorPublishAction : public Action<Ts...> {
 public:
  TextSensorPublishAction(TextSensor *sensor);
  template<typename V> void set_state(V state) { this->state_ = state; }
  void play(Ts... x) override;

 protected:
  TextSensor *sensor_;
  TemplatableValue<std::string, Ts...> state_;
};

template<typename... Ts>
TextSensorPublishAction<Ts...>::TextSensorPublishAction(TextSensor *sensor) : sensor_(sensor) {}
template<typename... Ts> void TextSensorPublishAction<Ts...>::play(Ts... x) {
  this->sensor_->publish_state(this->state_.value(x...));
  this->play_next(x...);
}
template<typename... Ts> TextSensorPublishAction<Ts...> *TextSensor::make_text_sensor_publish_action() {
  return new TextSensorPublishAction<Ts...>(this);
}

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#include "esphome/text_sensor/mqtt_text_sensor.h"

#endif  // USE_TEXT_SENSOR

#endif  // ESPHOME_TEXT_SENSOR_TEXT_SENSOR_H
