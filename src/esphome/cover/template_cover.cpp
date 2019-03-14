#include "esphome/defines.h"

#ifdef USE_TEMPLATE_COVER

#include "esphome/cover/template_cover.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

static const char *TAG = "cover.template";

TemplateCover::TemplateCover(const std::string &name)
    : Cover(name), open_trigger_(new Trigger<>()), close_trigger_(new Trigger<>()), stop_trigger_(new Trigger<>()), \
	  position_trigger_(new Trigger<>()), tilt_trigger_(new Trigger<>()){}
void TemplateCover::loop() {
  if (!this->f_.has_value())
    return;
  auto s = (*this->f_)();
  if (!s.has_value())
    return;

  this->publish_state(*s);

  if (!this->p_.has_value())
    return;
  auto ps = (*this->p_)();
  if (!ps.has_value())
    return;

  this->publish_position(*ps);

  if (!this->t_.has_value())
    return;
  auto ts = (*this->t_)();
  if (!ts.has_value())
    return;

  this->publish_tilt(*ts);
}
void TemplateCover::set_assumed_state(bool assumed_state) { this->assumed_state_ = assumed_state; }
bool TemplateCover::assumed_state() { return this->assumed_state_; }
void TemplateCover::set_state_lambda(std::function<optional<CoverState>()> &&f) { this->f_ = f; }
void TemplateCover::set_position_lambda(std::function<optional<position_value_t>()> &&p) { this->p_ = p; }
void TemplateCover::set_tilt_lambda(std::function<optional<tilt_value_t>()> &&t) { this->t_ = t; }
float TemplateCover::get_setup_priority() const { return setup_priority::HARDWARE; }
Trigger<> *TemplateCover::get_open_trigger() const { return this->open_trigger_; }
Trigger<> *TemplateCover::get_close_trigger() const { return this->close_trigger_; }
Trigger<> *TemplateCover::get_stop_trigger() const { return this->stop_trigger_; }
Trigger<> *TemplateCover::get_position_trigger() const { return this->position_trigger_; }
Trigger<> *TemplateCover::get_tilt_trigger() const { return this->tilt_trigger_; }
void TemplateCover::write_command(CoverCommand command) {
  if (this->prev_trigger_ != nullptr) {
    this->prev_trigger_->stop();
  }
  switch (command) {
    case COVER_COMMAND_OPEN: {
      this->prev_trigger_ = this->open_trigger_;
      this->open_trigger_->trigger();
      if (this->optimistic_)
        this->publish_state(COVER_OPEN);
      break;
    }
    case COVER_COMMAND_CLOSE: {
      this->prev_trigger_ = this->close_trigger_;
      this->close_trigger_->trigger();
      if (this->optimistic_)
        this->publish_state(COVER_CLOSED);
      break;
    }
    case COVER_COMMAND_STOP: {
      this->prev_trigger_ = this->stop_trigger_;
      this->stop_trigger_->trigger();
      break;
    }
    case COVER_COMMAND_POSITION: {
      break;
    }
    case COVER_COMMAND_TILT: {
      break;
    }
  }
}

void TemplateCover::write_command(CoverCommand command, float value) {
  switch (command) {
    case COVER_COMMAND_OPEN: {
      break;
    }
    case COVER_COMMAND_CLOSE: {
      break;
    }
    case COVER_COMMAND_STOP: {
      break;
    }
    case COVER_COMMAND_POSITION: {
      this->position_value = value;
      this->prev_trigger_ = this->position_trigger_;
      this->position_trigger_->trigger();
      ESP_LOGD(TAG, "Template Cover: Triggering position update to %f", value);
      if (this->optimistic())
        this->publish_position(position_value);
      break;
    }
    case COVER_COMMAND_TILT: {
      this->tilt_value = value;
      this->prev_trigger_ = this->tilt_trigger_;
      this->tilt_trigger_->trigger();
      ESP_LOGD(TAG, "Template Cover: Triggering tilt update to %f", value);
      if (this->optimistic())
        this->publish_tilt(tilt_value);
      break;
    }
  }
}

void TemplateCover::dump_config() { LOG_COVER("", "Template Cover", this); }

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_COVER
