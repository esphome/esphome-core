#include "esphome/defines.h"

#ifdef USE_COVER

#include "esphome/cover/cover.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

static const char *TAG = "cover";

const char *cover_state_to_str(CoverState state) {
  switch (state) {
    case COVER_OPEN:
      return "OPEN";
    case COVER_CLOSED:
      return "CLOSED";
    default:
      return "UNKNOWN";
  }
}
const char *cover_command_to_str(CoverCommand command) {
  switch (command) {
    case COVER_COMMAND_OPEN:
      return "OPEN";
    case COVER_COMMAND_CLOSE:
      return "CLOSE";
    case COVER_COMMAND_STOP:
      return "STOP";
    default:
      return "UNKNOWN";
  }
}

Cover::Cover(const std::string &name) : Nameable(name) {}

uint32_t Cover::hash_base() { return 1727367479UL; }

#ifdef USE_MQTT_COVER
MQTTCoverComponent *Cover::get_mqtt() const { return this->mqtt_; }
void Cover::set_mqtt(MQTTCoverComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

CoverCall::CoverCall(Cover *parent) : parent_(parent) {}
CoverCall &CoverCall::set_command(CoverCommand command) {
  this->command_ = command;
  return *this;
}
CoverCall &CoverCall::set_command(const char *command) {
  if (strcasecmp(command, "OPEN") == 0) {
    this->command_ = COVER_COMMAND_OPEN;
  } else if (strcasecmp(command, "CLOSE") == 0) {
    this->command_ = COVER_COMMAND_CLOSE;
  } else if (strcasecmp(command, "STOP") == 0) {
    this->command_ = COVER_COMMAND_STOP;
  } else {
    ESP_LOGW(TAG, "'%s' - Unrecognized command %s", this->parent_->get_name().c_str(), command);
  }
  return *this;
}
CoverCall &CoverCall::set_command_open() {
  this->command_ = COVER_COMMAND_OPEN;
  return *this;
}
CoverCall &CoverCall::set_command_close() {
  this->command_ = COVER_COMMAND_CLOSE;
  return *this;
}
CoverCall &CoverCall::set_command_stop() {
  this->command_ = COVER_COMMAND_STOP;
  return *this;
}
CoverCall &CoverCall::set_position(float position) {
  auto traits = this->parent_->get_traits();
  if (!traits.get_supports_position()) {
    ESP_LOGW(TAG, "'%s' - This climate device does not support position!", this->parent_->get_name().c_str());
    return *this;
  }
  this->position_ = clamp(0.0f, 1.0f, position);
  return *this;
}
CoverCall &CoverCall::set_tilt(float tilt) {
  auto traits = this->parent_->get_traits();
  if (!traits.get_supports_tilt()) {
    ESP_LOGW(TAG, "'%s' - This climate device does not support tilt!", this->parent_->get_name().c_str());
    return *this;
  }
  this->tilt_ = clamp(0.0f, 1.0f, tilt);
  return *this;
}
void CoverCall::perform() const {
  ESP_LOGD(TAG, "'%s' - Setting", this->parent_->get_name().c_str());
  if (this->command_.has_value()) {
    ESP_LOGD(TAG, "  Command: %s", cover_command_to_str(*this->command_));
  }
  if (this->position_.has_value()) {
    ESP_LOGD(TAG, "  Position: %.1f%%", *this->position_ * 100.0f);
  }
  if (this->tilt_.has_value()) {
    ESP_LOGD(TAG, "  Tilt: %.0f%%", *this->tilt_ * 100.0f);
  }
  this->parent_->control(*this);
}
const optional<CoverCommand> &CoverCall::get_command() const { return this->command_; }
const optional<float> &CoverCall::get_position() const { return this->position_; }
const optional<float> &CoverCall::get_tilt() const { return this->tilt_; }
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
  this->state_callback_.call();
  CoverRestoreState save;
  save.state = this->state;
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

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_COVER
