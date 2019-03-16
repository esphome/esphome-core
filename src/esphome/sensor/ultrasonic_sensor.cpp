#include "esphome/defines.h"

#ifdef USE_ULTRASONIC_SENSOR

#include "esphome/sensor/ultrasonic_sensor.h"

#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.ultrasonic";

UltrasonicSensorComponent::UltrasonicSensorComponent(const std::string &name, GPIOPin *trigger_pin, GPIOPin *echo_pin,
                                                     uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), trigger_pin_(trigger_pin), echo_pin_(echo_pin) {}
void UltrasonicSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Ultrasonic Sensor...");
  this->echo_pin_->setup();
  this->trigger_pin_->setup();
  this->trigger_pin_->digital_write(false);
  this->echo_pin_->attach_interrupt(UltrasonicSensorStore::gpio_intr, &this->store_, RISING);
}
void ICACHE_RAM_ATTR UltrasonicSensorStore::gpio_intr(UltrasonicSensorStore *arg) {
  if (arg->has_echo || !arg->has_triggered)
    // already have echo or no trigger
    return;

  arg->has_echo = true;
  arg->echo_at = micros();
}
void UltrasonicSensorComponent::update() {
  this->trigger_pin_->digital_write(true);
  delayMicroseconds(this->pulse_time_us_);
  this->trigger_pin_->digital_write(false);

  this->store_.has_triggered = true;
  this->store_.has_echo = false;
  this->store_.triggered_at = micros();
}
void UltrasonicSensorComponent::loop() {
  if (this->store_.has_triggered && this->store_.has_echo) {
    this->store_.has_triggered = false;
    uint32_t d_us = this->store_.echo_at - this->store_.triggered_at;

    float result = us_to_m(d_us);
    ESP_LOGD(TAG, "Got distance: %.1f m", result);
    this->publish_state(result);
  }
  uint32_t now = micros();
  if (this->store_.has_triggered && !this->store_.has_echo && now - this->store_.triggered_at > this->timeout_us_) {
    // timeout
    this->store_.has_triggered = false;
    ESP_LOGD(TAG, "Distance measurement timed out!");
    this->publish_state(NAN);
  }
}
void UltrasonicSensorComponent::dump_config() {
  LOG_SENSOR("", "Ultrasonic Sensor", this);
  LOG_PIN("  Echo Pin: ", this->echo_pin_);
  LOG_PIN("  Trigger Pin: ", this->trigger_pin_);
  ESP_LOGCONFIG(TAG, "  Pulse time: %u µs", this->pulse_time_us_);
  ESP_LOGCONFIG(TAG, "  Timeout: %u µs", this->timeout_us_);
  LOG_UPDATE_INTERVAL(this);
}
float UltrasonicSensorComponent::us_to_m(uint32_t us) {
  // The ultrasonic sound wave needs to travel both ways.
  return (SPEED_OF_SOUND_M_PER_US / 2.0f) * us;
}
float UltrasonicSensorComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
void UltrasonicSensorComponent::set_pulse_time_us(uint32_t pulse_time_us) { this->pulse_time_us_ = pulse_time_us; }
std::string UltrasonicSensorComponent::unit_of_measurement() { return "m"; }
std::string UltrasonicSensorComponent::icon() { return "mdi:arrow-expand-vertical"; }
int8_t UltrasonicSensorComponent::accuracy_decimals() {
  return 2;  // cm precision
}
void UltrasonicSensorComponent::set_timeout_us(uint32_t timeout_us) { this->timeout_us_ = timeout_us; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_ULTRASONIC_SENSOR
