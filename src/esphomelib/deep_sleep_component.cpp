//
//  deep_sleep_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 15.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include <Esp.h>
#include "esphomelib/deep_sleep_component.h"
#include "esphomelib/log.h"

namespace esphomelib {

static const char *TAG = "deep_sleep";

void DeepSleepComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Deep Sleep...");
  if (this->sleep_duration_)
    ESP_LOGCONFIG(TAG, "  Sleep Duration: %u ms", this->sleep_duration_.value / 1000);
  if (this->run_duration_)
    ESP_LOGCONFIG(TAG, "  Run Duration: %u ms", this->run_duration_.value);
  if (this->loop_cycles_)
    ESP_LOGCONFIG(TAG, "  Loop Cycles: %u", this->loop_cycles_.value);
#ifdef ARDUINO_ARCH_ESP32
  if (this->wakeup_pin_)
    ESP_LOGCONFIG(TAG, "  Wakeup Pin: %u %s", this->wakeup_pin_.value, this->wakeup_level_ ? "HIGH" : "LOW");
#endif
  if (this->run_duration_)
    this->set_timeout("sleep", this->run_duration_.value, [this](){
      this->begin_sleep();
    });
}
void DeepSleepComponent::loop() {
  if (this->loop_cycles_) {
    if (++this->at_loop_cycle_ >= this->loop_cycles_.value)
      this->begin_sleep();
  }
}
float DeepSleepComponent::get_loop_priority() const {
  return -100.0f; // run after everything else is ready
}
void DeepSleepComponent::set_sleep_duration(uint32_t time_ms) {
  this->sleep_duration_ = uint64_t(time_ms) * 1000;
}
#ifdef ARDUINO_ARCH_ESP32
void DeepSleepComponent::set_wakeup_pin(uint8_t pin, bool level) {
  this->wakeup_pin_ = pin;
  this->wakeup_level_ = level;
}
#endif
void DeepSleepComponent::set_loop_cycles(uint32_t cycles) {
  this->loop_cycles_ = cycles;
}
void DeepSleepComponent::set_run_duration(uint32_t time_ms) {
  this->run_duration_ = time_ms;
}
void DeepSleepComponent::begin_sleep() {
  ESP_LOGI(TAG, "Beginning Deep Sleep");
#ifdef ARDUINO_ARCH_ESP32
  if (this->sleep_duration_)
    esp_sleep_enable_timer_wakeup(this->sleep_duration_.value);
  if (this->wakeup_pin_)
    esp_sleep_enable_ext0_wakeup(gpio_num_t(this->wakeup_pin_.value), this->wakeup_level_);
  esp_deep_sleep_start();
#endif

#ifdef ARDUINO_ARCH_ESP8266
  ESP.deepSleep(this->sleep_duration_);
#endif
}

} // namespace esphomelib
