#include "esphomelib/defines.h"

#ifdef USE_YX75V18

#include "esphomelib/dc_motor/yx75v18.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace dc_motor {

static const char *TAG = "dc_motor.yx75v18";

void YX75V18::setup() {
  ESP_LOGCONFIG(TAG, "Setting up YX75V18...");
//   if (this->sleep_pin_ != nullptr) {
//     this->sleep_pin_->setup();
//     this->sleep_pin_->digital_write(false);
//   }
//   this->step_pin_->setup();
//   this->step_pin_->digital_write(false);
//   this->dir_pin_->setup();
//   this->dir_pin_->digital_write(false);
}
void YX75V18::dump_config() {
//   ESP_LOGCONFIG(TAG, "A4988:");
//   LOG_PIN("  Step Pin: ", this->step_pin_);
//   LOG_PIN("  Dir Pin: ", this->dir_pin_);
//   LOG_PIN("  Sleep Pin: ", this->sleep_pin_);
//   LOG_STEPPER(this);
}
void YX75V18::loop() {
//   bool at_target = this->has_reached_target();
//   if (this->sleep_pin_ != nullptr) {
//     this->sleep_pin_->digital_write(!at_target);
//   }
//   if (at_target) {
//     this->high_freq_.stop();
//   } else {
//     this->high_freq_.start();
//   }

//   int32_t dir = this->should_step_();
//   if (dir == 0)
//     return;

//   this->dir_pin_->digital_write(dir == 1);
//   this->step_pin_->digital_write(true);
//   delayMicroseconds(5);
//   this->step_pin_->digital_write(false);
}
float YX75V18::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
YX75V18::YX75V18(GPIOPin *positive_pin, GPIOPin *negative_pin) {
    this->positive_pin_ = positive_pin;
    this->negative_pin_ = negative_pin;
}

// void YX75V18::set_sleep_pin(const GPIOOutputPin &sleep_pin) {
//   this->sleep_pin_ = sleep_pin.copy();
// }

} // namespace stepper

ESPHOMELIB_NAMESPACE_END

#endif //USE_A4988
