#include "esphome/defines.h"

#ifdef USE_TEMPLATE_COVER

#include "esphome/cover/template_cover.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

static const char *TAG = "cover.template";

TemplateCover::TemplateCover(const std::string &name)
    : Cover(name), open_trigger_(new Trigger<>()), close_trigger_(new Trigger<>), stop_trigger_(new Trigger<>()) {}
void TemplateCover::loop() {
  if (!this->f_.has_value())
    return;
  auto s = (*this->f_)();
  if (!s.has_value())
    return;

  this->publish_state(*s);
}
void TemplateCover::set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
void TemplateCover::set_assumed_state(bool assumed_state) { this->assumed_state_ = assumed_state; }
bool TemplateCover::assumed_state() { return this->assumed_state_; }
void TemplateCover::set_state_lambda(std::function<optional<CoverState>()> &&f) { this->f_ = f; }
float TemplateCover::get_setup_priority() const { return setup_priority::HARDWARE; }
Trigger<> *TemplateCover::get_open_trigger() const { return this->open_trigger_; }
Trigger<> *TemplateCover::get_close_trigger() const { return this->close_trigger_; }
Trigger<> *TemplateCover::get_stop_trigger() const { return this->stop_trigger_; }
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
  }
}
void TemplateCover::dump_config() { LOG_COVER("", "Template Cover", this); }

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_COVER
