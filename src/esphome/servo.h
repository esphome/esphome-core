#ifndef ESPHOME_CORE_SERVO_H
#define ESPHOME_CORE_SERVO_H

#include "esphome/defines.h"

#ifdef USE_SERVO

#include "esphome/component.h"
#include "esphome/output/float_output.h"

ESPHOME_NAMESPACE_BEGIN

class Servo : public Component {
 public:
  Servo(output::FloatOutput *output);
  void write(float value);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_min_level(float min_level);
  void set_idle_level(float idle_level);
  void set_max_level(float max_level);

 protected:
  output::FloatOutput *output_;
  float min_level_ = 0.0300f;
  float idle_level_ = 0.0750f;
  float max_level_ = 0.1200f;
};

template<typename... Ts> class ServoWriteAction : public Action<Ts...> {
 public:
  ServoWriteAction(Servo *servo);
  template<typename V> void set_value(V val) { this->value_ = val; }
  void play(Ts... x) override;

 protected:
  Servo *servo_;
  TemplatableValue<float, Ts...> value_;
};

template<typename... Ts> ServoWriteAction<Ts...>::ServoWriteAction(Servo *servo) : servo_(servo) {}
template<typename... Ts> void ServoWriteAction<Ts...>::play(Ts... x) {
  this->servo_->write(this->value_.value(x...));
  this->play_next(x...);
}

ESPHOME_NAMESPACE_END

#endif  // USE_SERVO

#endif  // ESPHOME_CORE_SERVO_H
