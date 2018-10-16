#ifndef ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H
#define ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_TEXT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/automation.h"
#include "esphomelib/helpers.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class TextSensorValueTrigger;

class TextSensor : public Nameable {
 public:
  explicit TextSensor(const std::string &name) : Nameable(name) {}

  void push_new_value(std::string value);

  void set_icon(const std::string &icon);

  void add_on_value_callback(std::function<void(std::string)> callback);

  std::string value;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  std::string get_icon();

  virtual std::string icon();

  virtual std::string unique_id();

  TextSensorValueTrigger *make_value_trigger();

  bool has_value();

 protected:
  CallbackManager<void(std::string)> callback_;
  optional<std::string> icon_;
  bool has_value_{false};
};

class TextSensorValueTrigger : public Trigger<std::string> {
 public:
  explicit TextSensorValueTrigger(TextSensor *parent);
};

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEXT_SENSOR

#endif //ESPHOMELIB_TEXT_SENSOR_TEXT_SENSOR_H
