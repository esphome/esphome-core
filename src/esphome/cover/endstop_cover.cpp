#include "esphome/defines.h"

#ifdef USE_ENDSTOP_COVER

#include "esphome/cover/endstop_cover.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

static const char *TAG = "cover.endstop";

CoverTraits EndstopCover::get_traits() {
  auto traits = CoverTraits();
  traits.set_supports_position(true);
  traits.set_is_assumed_state(false);
  return traits;
}
void EndstopCover::control(const CoverCall &call) {
  if (call.get_stop()) {
    this->start_direction_(COVER_OPERATION_IDLE);
    this->publish_state();
  }
  if (call.get_position().has_value()) {
    auto pos = *call.get_position();
    if (pos == COVER_OPEN) {
      if (this->is_open_()) {
        ESP_LOGW(TAG, "'%s' - Cover is already open!", this->name_.c_str());
        return;
      }

      this->start_direction_(COVER_OPERATION_IS_OPENING);
    } else if (pos == COVER_CLOSED) {
      if (this->is_closed_()) {
        ESP_LOGW(TAG, "'%s' - Cover is already closed!", this->name_.c_str());
        return;
      }

      this->start_direction_(COVER_OPERATION_IS_CLOSING);
    } else {
      auto op = pos < this->position ? COVER_OPERATION_IS_CLOSING : COVER_OPERATION_IS_OPENING;
      this->start_direction_(op);
    }
  }
}
void EndstopCover::setup() {
  auto restore = this->restore_state_();
  if (restore.has_value()) {
    restore->apply(this);
  }

  if (this->is_open_()) {
    this->position = COVER_OPEN;
  } else if (this->is_closed_()) {
    this->position = COVER_CLOSED;
  } else if (!restore.has_value()) {
    this->position = 0.5f;
  }
}
void EndstopCover::loop() {
  if (this->current_operation == COVER_OPERATION_IDLE)
    return;

  const uint32_t now = millis();
  if (this->current_operation == COVER_OPERATION_IS_OPENING && this->is_open_()) {
    float dur = (now - this->start_dir_time_) / 1e3f;
    ESP_LOGD(TAG, "'%s' - Open endstop reached. Took %.1f seconds.", this->name_.c_str(), dur);

    this->start_direction_(COVER_OPERATION_IDLE);
    this->position = COVER_OPEN;
    this->publish_state();
  }
  if (this->current_operation == COVER_OPERATION_IS_CLOSING && this->is_closed_()) {
    float dur = (now - this->start_dir_time_) / 1e3f;
    ESP_LOGD(TAG, "'%s' - Close endstop reached. Took %.1f seconds.", this->name_.c_str(), dur);

    this->start_direction_(COVER_OPERATION_IDLE);
    this->position = COVER_CLOSED;
    this->publish_state();
  }

  // Recompute position every loop cycle
  this->recompute_position_();

  // Send current position every second
  if (this->current_operation != COVER_OPERATION_IDLE && now - this->last_publish_time_ > 1000) {
    this->publish_state();
    this->last_publish_time_ = now;
  }
}
void EndstopCover::dump_config() {
  LOG_COVER("", "Endstop Cover", this);
}
float EndstopCover::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

} // namespace cover

ESPHOME_NAMESPACE_END

#endif //USE_ENDSTOP_COVER
