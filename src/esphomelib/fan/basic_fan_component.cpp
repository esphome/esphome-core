//
// Created by Otto Winter on 30.12.17.
//

#include "esphomelib/fan/basic_fan_component.h"

#include "esphomelib/log.h"

#ifdef USE_FAN

namespace esphomelib {

namespace fan {

static const char *TAG = "fan.basic_fan";

void BasicFanComponent::set_binary(output::BinaryOutput *output) {
  this->binary_output_ = output;
}
void BasicFanComponent::set_speed(output::FloatOutput *output, float off_speed, float low_speed, float medium_speed, float high_speed) {
  FanTraits traits = this->state_->get_traits();
  traits.set_speed(true);
  this->state_->set_traits(traits);

  this->speed_output_ = output;
  this->off_speed_ = off_speed;
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
FanState *BasicFanComponent::get_state() const {
  return this->state_;
}
void BasicFanComponent::set_state(FanState *state) {
  this->state_ = state;
}
void BasicFanComponent::setup() {
  this->state_->add_on_receive_frontend_state_callback([this]() { this->next_update_ = true; });
}
void BasicFanComponent::loop() {
  if (this->next_update_) {
    this->next_update_ = false;

    if (this->state_->get_traits().supports_speed()) {
      float speed = this->off_speed_;
      if (this->state_->get_state()) {
        if (this->state_->get_speed() == FanState::SPEED_LOW)
          speed = this->low_speed_;
        else if (this->state_->get_speed() == FanState::SPEED_MEDIUM)
          speed = this->medium_speed_;
        else if (this->state_->get_speed() == FanState::SPEED_HIGH)
          speed = this->high_speed_;
      }
      ESP_LOGD(TAG, "Setting speed: %.2f", speed);
      this->speed_output_->set_state_(speed);
    }
    if (this->binary_output_ != nullptr) {
      bool enable = this->state_->get_state();
      if (enable) this->binary_output_->enable();
      else this->binary_output_->disable();
      ESP_LOGD(TAG, "Setting binary state: %d", int(enable));
    }

    if (this->state_->get_traits().supports_oscillation()) {
      bool enable = this->state_->is_oscillating();
      if (enable) this->binary_output_->enable();
      else this->binary_output_->disable();
      ESP_LOGD(TAG, "Setting oscillation: %d", int(enable));
    }
  }
}

} // namespace fan

} // namespace esphomelib

#endif //USE_FAN
