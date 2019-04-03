#include "esphome/defines.h"

#ifdef USE_COVER

#include "esphome/cover/cover.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

static const char *TAG = "cover";

const float COVER_OPEN = 1.0f;
const float COVER_CLOSED = 0.0f;

const char *cover_command_to_str(float pos) {
  if (pos == COVER_OPEN) {
    return "OPEN";
  } else if (pos == COVER_CLOSED) {
    return "CLOSE";
  } else {
    return "UNKNOWN";
  }
}

Cover::Cover(const std::string &name) : Nameable(name), position{COVER_OPEN} {}

uint32_t Cover::hash_base() { return 1727367479UL; }

#ifdef USE_MQTT_COVER
MQTTCoverComponent *Cover::get_mqtt() const { return this->mqtt_; }
void Cover::set_mqtt(MQTTCoverComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

CoverCall::CoverCall(Cover *parent) : parent_(parent) {}
CoverCall &CoverCall::set_command(const char *command) {
  if (strcasecmp(command, "OPEN") == 0) {
    this->set_command_open();
  } else if (strcasecmp(command, "CLOSE") == 0) {
    this->set_command_close();
  } else if (strcasecmp(command, "STOP") == 0) {
    this->set_command_stop();
  } else {
    ESP_LOGW(TAG, "'%s' - Unrecognized command %s", this->parent_->get_name().c_str(), command);
  }
  return *this;
}
CoverCall &CoverCall::set_command_open() {
  this->position_ = COVER_OPEN;
  return *this;
}
CoverCall &CoverCall::set_command_close() {
  this->position_ = COVER_CLOSED;
  return *this;
}
CoverCall &CoverCall::set_command_stop() {
  this->stop_ = true;
  return *this;
}
CoverCall &CoverCall::set_position(float position) {
  this->position_ = position;
  return *this;
}
CoverCall &CoverCall::set_tilt(float tilt) {
  this->tilt_ = tilt;
  return *this;
}
void CoverCall::perform() {
  ESP_LOGD(TAG, "'%s' - Setting", this->parent_->get_name().c_str());
  auto traits = this->parent_->get_traits();
  this->validate_();
  if (this->stop_) {
    ESP_LOGD(TAG, "  Command: STOP");
  }
  if (this->position_.has_value()) {
    if (traits.get_supports_position()) {
      ESP_LOGD(TAG, "  Position: %.0f%%", *this->position_ * 100.0f);
    } else {
      ESP_LOGD(TAG, "  Command: %s", cover_command_to_str(*this->position_));
    }
  }
  if (this->tilt_.has_value()) {
    ESP_LOGD(TAG, "  Tilt: %.0f%%", *this->tilt_ * 100.0f);
  }
  this->parent_->control(*this);
}
const optional<float> &CoverCall::get_position() const { return this->position_; }
const optional<float> &CoverCall::get_tilt() const { return this->tilt_; }
void CoverCall::validate_() {
  auto traits = this->parent_->get_traits();
  if (this->position_.has_value()) {
    auto pos = *this->position_;
    if (!traits.get_supports_position() && pos != COVER_OPEN && pos != COVER_CLOSED) {
      ESP_LOGW(TAG, "'%s' - This cover device does not support setting position!", this->parent_->get_name().c_str());
      this->position_.reset();
    } else if (pos < 0.0f || pos > 1.0f) {
      ESP_LOGW(TAG, "'%s' - Position %.2f is out of range [0.0 - 1.0]", this->parent_->get_name().c_str(), pos);
      this->position_ = clamp(0.0f, 1.0f, pos);
    }
  }
  if (this->tilt_.has_value()) {
    auto tilt = *this->tilt_;
    if (!traits.get_supports_tilt()) {
      ESP_LOGW(TAG, "'%s' - This cover device does not support tilt!", this->parent_->get_name().c_str());
      this->tilt_.reset();
    } else if (tilt < 0.0f || tilt > 1.0f) {
      ESP_LOGW(TAG, "'%s' - Tilt %.2f is out of range [0.0 - 1.0]", this->parent_->get_name().c_str(), tilt);
      this->tilt_ = clamp(0.0f, 1.0f, tilt);
    }
  }
  if (this->stop_) {
    if (this->position_.has_value()) {
      ESP_LOGW(TAG, "Cannot set position when stopping a cover!");
      this->position_.reset();
    }
    if (this->tilt_.has_value()) {
      ESP_LOGW(TAG, "Cannot set tilt when stopping a cover!");
      this->tilt_.reset();
    }
  }
}
void Cover::set_device_class(const std::string &device_class) { this->device_class_override_ = device_class; }
CoverTraits Cover::get_traits() {
  auto traits = this->traits();
  if (!this->device_class_override_.empty()) {
    traits.set_device_class(this->device_class_override_);
  }
  return traits;
}
CoverCall Cover::make_call() { return CoverCall(this); }
void Cover::open() {
  auto call = this->make_call();
  call.set_command_open();
  call.perform();
}
void Cover::close() {
  auto call = this->make_call();
  call.set_command_close();
  call.perform();
}
void Cover::stop() {
  auto call = this->make_call();
  call.set_command_stop();
  call.perform();
}
void Cover::add_on_state_callback(std::function<void()> &&f) { this->state_callback_.add(std::move(f)); }
void Cover::publish_state() {
  this->position = clamp(0.0f, 1.0f, this->position);
  this->tilt = clamp(0.0f, 1.0f, this->tilt);

  this->state_callback_.call();

  CoverRestoreState save;
  save.position = this->position;
  save.tilt = this->tilt;
  this->rtc_.save(&save);
}
optional<CoverRestoreState> Cover::restore_state_() {
  this->rtc_ = global_preferences.make_preference<CoverRestoreState>(this->get_object_id_hash());
  CoverRestoreState recovered;
  if (!this->rtc_.load(&recovered))
    return {};
  return recovered;
}

CoverCall CoverRestoreState::to_call(Cover *cover) {
  auto call = cover->make_call();
  auto traits = cover->get_traits();
  call.set_position(this->position);
  if (traits.get_supports_tilt())
    call.set_tilt(this->tilt);
  return call;
}

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_COVER
