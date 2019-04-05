#ifndef ESPHOME_CORE_ENDSTOP_COVER_H
#define ESPHOME_CORE_ENDSTOP_COVER_H

#include "esphome/defines.h"

#ifdef USE_ENDSTOP_COVER

#include "esphome/component.h"
#include "esphome/cover/cover.h"
#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

class EndstopCover : public Cover, public Component {
 public:
  EndstopCover(const std::string &name) : Cover(name) {}
  CoverTraits get_traits() override;
  void control(const CoverCall &call) override;

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  Trigger<> *get_open_trigger() const {
    return this->open_trigger_;
  }
  Trigger<> *get_close_trigger() const {
    return this->close_trigger_;
  }
  Trigger<> *get_stop_trigger() const {
    return this->stop_trigger_;
  }
  void set_open_endstop(binary_sensor::BinarySensor *open_endstop) {
    this->open_endstop_ = open_endstop;
  }
  void set_close_endstop(binary_sensor::BinarySensor *close_endstop) {
    this->close_endstop_ = close_endstop;
  }
  void set_open_duration(uint32_t open_duration) {
    this->open_duration_ = open_duration;
  }
  void set_close_duration(uint32_t close_duration) {
    this->close_duration_ = close_duration;
  }

 protected:
  void stop_prev_trigger_() {
    if (this->prev_command_trigger_ != nullptr) {
      this->prev_command_trigger_->stop();
      this->prev_command_trigger_ = nullptr;
    }
  }
  bool is_open_() { return this->open_endstop_->state; }
  bool is_closed_() { return this->close_endstop_->state; }

  void start_direction_(CoverOperation dir) {
    if (dir == this->current_operation)
      return;

    this->recompute_position_();
    Trigger<> *trig;
    switch (dir) {
      case COVER_OPERATION_IDLE:
        trig = this->stop_trigger_;
        break;
      case COVER_OPERATION_IS_OPENING:
        trig = this->open_trigger_;
        break;
      case COVER_OPERATION_IS_CLOSING:
        trig = this->close_trigger_;
        break;
      default:
        return;
    }

    this->current_operation = dir;

    this->stop_prev_trigger_();
    trig->trigger();
    this->prev_command_trigger_ = trig;

    this->start_dir_time_ = millis();
  }

  void recompute_position_() {
    if (this->current_operation == COVER_OPERATION_IDLE)
      return;

    float dir;
    float action_dur;
    switch (this->current_operation) {
      case COVER_OPERATION_IS_OPENING:
        dir = 1.0f;
        action_dur = this->open_duration_;
        break;
      case COVER_OPERATION_IS_CLOSING:
        dir = -1.0f;
        action_dur = this->close_duration_;
        break;
      default:
        return;
    }

    const uint32_t now = millis();
    this->position += dir * (now - this->last_recompute_time_) / action_dur;
    this->position = clamp(0.0f, 1.0f, this->position);

    this->last_recompute_time_ = now;
  }

  binary_sensor::BinarySensor *open_endstop_;
  binary_sensor::BinarySensor *close_endstop_;
  Trigger<> *open_trigger_{new Trigger<>()};
  uint32_t open_duration_;
  Trigger<> *close_trigger_{new Trigger<>()};
  uint32_t close_duration_;
  Trigger<> *stop_trigger_{new Trigger<>()};
  Trigger<> *prev_command_trigger_{nullptr};
  uint32_t last_recompute_time_{0};
  uint32_t start_dir_time_{0};
  uint32_t last_publish_time_{0};
};

} // namespace cover

ESPHOME_NAMESPACE_END

#endif //USE_ENDSTOP_COVER

#endif //ESPHOME_CORE_ENDSTOP_COVER_H
