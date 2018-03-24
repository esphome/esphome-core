//
//  ultrasonic_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 21.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/input/ultrasonic_sensor.h"

#include "esphomelib/log.h"
#include "esphomelib/helpers.h"

namespace esphomelib {

namespace input {

static const char *TAG = "input::ultrasonic";

UltrasonicSensorComponent::UltrasonicSensorComponent(GPIOOutputPin trigger_pin,
                                                     GPIOInputPin echo_pin,
                                                     uint32_t update_interval)
    : PollingSensorComponent(update_interval),
      trigger_pin_(trigger_pin), echo_pin_(echo_pin) {

}
GPIOOutputPin &UltrasonicSensorComponent::get_trigger_pin() {
  return this->trigger_pin_;
}
void UltrasonicSensorComponent::set_trigger_pin(const GPIOOutputPin &trigger_pin) {
  this->trigger_pin_ = trigger_pin;
}
GPIOInputPin &UltrasonicSensorComponent::get_echo_pin() {
  return this->echo_pin_;
}
void UltrasonicSensorComponent::set_echo_pin(const GPIOInputPin &echo_pin) {
  this->echo_pin_ = echo_pin;
}
void UltrasonicSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Ultrasonic Sensor...");
  this->echo_pin_.setup();
  this->trigger_pin_.setup();
  this->trigger_pin_.write_value(false);
  ESP_LOGCONFIG(TAG, "    Pulse time: %uµs", this->pulse_time_us_);
  ESP_LOGCONFIG(TAG, "    Timeout: %uµs", this->timeout_us_);
}
void UltrasonicSensorComponent::update() {
  this->trigger_pin_.write_value(true);
  delayMicroseconds(this->pulse_time_us_);
  this->trigger_pin_.write_value(false);
  auto time = run_without_interrupts<uint32_t>([this] {
    return pulseIn(this->echo_pin_.get_pin(),
                   uint8_t(!this->echo_pin_.is_inverted()),
                   this->timeout_us_);
  });

  float result = 0;
  if (time == 0)
    result = NAN;
  else
    result = us_to_m(time);

  ESP_LOGV(TAG, "Echo took %uµs (%fm)", time, result);

  this->push_new_value(result);
}
uint32_t UltrasonicSensorComponent::get_timeout_us() const {
  return this->timeout_us_;
}
void UltrasonicSensorComponent::set_timeout_us(uint32_t timeout_us) {
  this->timeout_us_ = timeout_us;
}
void UltrasonicSensorComponent::set_timeout_m(float timeout_m) {
  this->set_timeout_us(m_to_us(timeout_m));
}
float UltrasonicSensorComponent::us_to_m(uint32_t us) {
  // The ultrasonic sound wave needs to travel both ways.
  return (SPEED_OF_SOUND_M_PER_US/2.0f) * us;
}
uint32_t UltrasonicSensorComponent::m_to_us(float m) {
  // The ultrasonic sound wave needs to travel both ways.
  return static_cast<uint32_t>(m / (SPEED_OF_SOUND_M_PER_US/2.0f));
}
float UltrasonicSensorComponent::get_timeout_m() const {
  return us_to_m(this->get_timeout_us());
}
float UltrasonicSensorComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
uint32_t UltrasonicSensorComponent::get_pulse_time_us() const {
  return this->pulse_time_us_;
}
void UltrasonicSensorComponent::set_pulse_time_us(uint32_t pulse_time_us) {
  this->pulse_time_us_ = pulse_time_us;
}
std::string UltrasonicSensorComponent::unit_of_measurement() {
  return sensor::UNIT_OF_MEASUREMENT_METER;
}
std::string UltrasonicSensorComponent::icon() {
  return sensor::ICON_DISTANCE;
}
int8_t UltrasonicSensorComponent::accuracy_decimals() {
  return 2; // cm precision
}

} // namespace input

} // namespace esphomelib
