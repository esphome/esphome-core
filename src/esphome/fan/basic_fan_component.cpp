#include "esphome/defines.h"

#ifdef USE_FAN

#include "esphome/fan/basic_fan_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace fan {

static const char *TAG = "fan.basic_fan";

void BasicFanComponent::set_binary(output::BinaryOutput *output) { this->binary_output_ = output; }
void BasicFanComponent::set_speed(output::FloatOutput *output, float low_speed, float medium_speed, float high_speed) {
  FanTraits traits = this->state_->get_traits();
  traits.set_speed(true);
  this->state_->set_traits(traits);

  this->speed_output_ = output;
  this->low_speed_ = low_speed;
  this->medium_speed_ = medium_speed;
  this->high_speed_ = high_speed;
}
void BasicFanComponent::set_oscillation(output::BinaryOutput *oscillating_output) {
  FanTraits traits = this->state_->get_traits();
  traits.set_oscillation(true);
  this->state_->set_traits(traits);

  this->oscillating_output_ = oscillating_output;
}
FanState *BasicFanComponent::get_state() const { return this->state_; }
void BasicFanComponent::set_state(FanState *state) { this->state_ = state; }
void BasicFanComponent::setup() {
  this->state_->add_on_state_callback([this]() { this->next_update_ = true; });
}
void BasicFanComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Fan '%s':", this->state_->get_name().c_str());
  if (this->state_->get_traits().supports_oscillation()) {
    ESP_LOGCONFIG(TAG, "  Oscillation: YES");
  }
  if (this->state_->get_traits().supports_speed()) {
    ESP_LOGCONFIG(TAG, "  Mode: Speed");
    ESP_LOGCONFIG(TAG, "  Speeds: Low=%.0f%% Medium=%.0f%% High=%.0f%%", this->low_speed_ * 100.0f,
                  this->medium_speed_ * 100.0f, this->high_speed_ * 100.0f);
  } else {
    ESP_LOGCONFIG(TAG, "  Mode: Binary");
  }
}
void BasicFanComponent::loop() {
  if (!this->next_update_) {
    return;
  }
  this->next_update_ = false;

  if (this->state_->get_traits().supports_speed()) {
    float speed = 0.0f;
    if (this->state_->state) {
      if (this->state_->speed == FAN_SPEED_LOW)
        speed = this->low_speed_;
      else if (this->state_->speed == FAN_SPEED_MEDIUM)
        speed = this->medium_speed_;
      else if (this->state_->speed == FAN_SPEED_HIGH)
        speed = this->high_speed_;
    }
    ESP_LOGD(TAG, "Setting speed: %.2f", speed);
    this->speed_output_->set_level(speed);
  }
  if (this->binary_output_ != nullptr) {
    bool enable = this->state_->state;
    if (enable)
      this->binary_output_->turn_on();
    else
      this->binary_output_->turn_off();
    ESP_LOGD(TAG, "Setting binary state: %d", int(enable));
  }

  if (this->state_->get_traits().supports_oscillation()) {
    bool enable = this->state_->oscillating;
    if (enable) {
      this->oscillating_output_->turn_on();
    } else {
      this->oscillating_output_->turn_off();
    }
    ESP_LOGD(TAG, "Setting oscillation: %d", int(enable));
  }
}
float BasicFanComponent::get_setup_priority() const { return setup_priority::HARDWARE; }

}  // namespace fan

ESPHOME_NAMESPACE_END

#endif  // USE_FAN
