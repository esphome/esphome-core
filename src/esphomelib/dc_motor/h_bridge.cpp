//Simple H-bridge chips:
//  https://slideplayer.com/slide/9387972/28/images/13/H-Bridge+Examples+L293%2C+L298%2C+MC33886%2C+TLE5206%2C+TPIC0108b%2C+etc.jpg

//Advanced H-bridge chips:
// https://slideplayer.com/slide/9387972/28/images/14/H-Bridge+Examples+LM18200%2C+TLE5205%2C+TPIC0107b%2C+etc..jpg

#include "esphomelib/defines.h"

#ifdef USE_H_BRIDGE

#include "esphomelib/dc_motor/h_bridge.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace dc_motor {

static const char *TAG = "dc_motor.h_bridge";

void H_bridge::setup() {
  ESP_LOGCONFIG(TAG, "Setting up h_bridge...");
  if (this->positive_pin_ == nullptr) 
    ESP_LOGW(TAG, "Positive_pin is null!");
  if (this->negative_pin_ == nullptr) 
    ESP_LOGW(TAG, "Negative_pin is null!");  

  this->positive_pin_->setup();
  this->negative_pin_->setup();
  this->stop();
}
void H_bridge::dump_config() {
  ESP_LOGCONFIG(TAG, "H_Bridge:");
  LOG_PIN("  Positive Pin: ", this->positive_pin_);
  LOG_PIN("  Negative Pin: ", this->negative_pin_);
}
void H_bridge::loop() {

}
float H_bridge::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
H_bridge::H_bridge(GPIOPin *positive_pin, GPIOPin *negative_pin) {
    this->positive_pin_ = positive_pin;
    this->negative_pin_ = negative_pin;
}

void H_bridge::move() {
    this->positive_pin_->digital_write(direction_clockwise_);
    this->negative_pin_->digital_write(!direction_clockwise_);
}

void H_bridge::stop() {
  this->positive_pin_->digital_write(LOW);
  this->negative_pin_->digital_write(LOW);
}
} // namespace dc_motor

ESPHOMELIB_NAMESPACE_END

#endif //USE_H_BRIDGE
