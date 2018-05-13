//
//  fast_led_light_output.cpp
//  esphomelib
//
//  Created by Otto Winter on 13.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/light/fast_led_light_output.h"
#include "esphomelib/log.h"

#ifdef USE_FAST_LED_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

static const char *TAG = "light.fast_led";

LightTraits FastLEDLightOutputComponent::get_traits() {
  return {true, true, false};
}
void FastLEDLightOutputComponent::write_state(LightState *state) {
  if (this->prevent_writing_leds_)
    return;

  float red, green, blue;
  state->current_values_as_rgb(&red, &green, &blue);
  CRGB crgb = CRGB(red * 255, green * 255, blue * 255);

  for (int i = 0; i < this->num_leds_; i++)
    this->leds_[i] = crgb;

  this->schedule_show();
}
void FastLEDLightOutputComponent::setup() {
  assert(this->controller_ != nullptr && "You need to add LEDs to this controller!");
  this->controller_->init();
  this->controller_->setLeds(this->leds_, this->num_leds_);
  if (!this->max_refresh_rate_.defined) {
    this->set_max_refresh_rate(this->controller_->getMaxRefreshRate());
  }
}
void FastLEDLightOutputComponent::loop() {
  if (!this->next_show_)
    return;

  uint32_t now = micros();
  // protect from refreshing too often
  if (this->max_refresh_rate_.value != 0 && (now - this->last_refresh_) < this->max_refresh_rate_.value) {
    return;
  }
  this->last_refresh_ = now;
  this->next_show_ = false;

  this->controller_->showLeds();
}
void FastLEDLightOutputComponent::schedule_show() {
  this->next_show_ = true;
}
CLEDController &FastLEDLightOutputComponent::add_leds(CLEDController *controller, int num_leds) {
  assert(this->controller_ == nullptr && "FastLEDLightOutputComponent only supports one controller at a time.");

  this->controller_ = controller;
  this->num_leds_ = num_leds;
  this->leds_ = new CRGB[num_leds];

  for (int i = 0; i < this->num_leds_; i++)
    this->leds_[i] = CRGB::Black;

  return *this->controller_;
}
CRGB *FastLEDLightOutputComponent::get_leds() const {
  return this->leds_;
}
CLEDController *FastLEDLightOutputComponent::get_controller() const {
  return this->controller_;
}
void FastLEDLightOutputComponent::set_max_refresh_rate(uint32_t interval_us) {
  this->max_refresh_rate_.value = interval_us;
  this->max_refresh_rate_.defined = true;
}
int FastLEDLightOutputComponent::get_num_leds() const {
  return this->num_leds_;
}
void FastLEDLightOutputComponent::unprevent_writing_leds() {
  this->prevent_writing_leds_ = false;
}
void FastLEDLightOutputComponent::prevent_writing_leds() {
  this->prevent_writing_leds_ = true;
}
float FastLEDLightOutputComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAST_LED_LIGHT
