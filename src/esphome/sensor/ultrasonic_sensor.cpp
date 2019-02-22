#include "esphome/defines.h"

#ifdef USE_ULTRASONIC_SENSOR

#include "esphome/sensor/ultrasonic_sensor.h"

#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.ultrasonic";

UltrasonicSensorComponent::UltrasonicSensorComponent(const std::string &name,
                                                     GPIOPin *trigger_pin, GPIOPin *echo_pin,
                                                     uint32_t update_interval)
    : PollingSensorComponent(name, update_interval),
      trigger_pin_(trigger_pin), echo_pin_(echo_pin) {

}
void UltrasonicSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Ultrasonic Sensor...");
  this->echo_pin_->setup();
  this->trigger_pin_->setup();
  this->trigger_pin_->digital_write(false);
}
void UltrasonicSensorComponent::dump_config() {
  LOG_SENSOR("", "Ultrasonic Sensor", this);
  LOG_PIN("  Echo Pin: ", this->echo_pin_);
  LOG_PIN("  Trigger Pin: ", this->trigger_pin_);
  ESP_LOGCONFIG(TAG, "    Pulse time: %u µs", this->pulse_time_us_);
  ESP_LOGCONFIG(TAG, "    Timeout: %u µs", this->timeout_us_);
  LOG_UPDATE_INTERVAL(this);
}
void UltrasonicSensorComponent::update() {
  this->trigger_pin_->digital_write(true);
  delayMicroseconds(this->pulse_time_us_);
  this->trigger_pin_->digital_write(false);
  disable_interrupts();
  uint32_t time = pulseIn(this->echo_pin_->get_pin(),
                          uint8_t(!this->echo_pin_->is_inverted()),
                          this->timeout_us_);
  enable_interrupts();

  float result = 0;
  if (time == 0)
    result = NAN;
  else
    result = us_to_m(time);

  ESP_LOGV(TAG, "Echo took %uµs (%fm)", time, result);

  this->publish_state(result);
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
  return "m";
}
std::string UltrasonicSensorComponent::icon() {
  return "mdi:arrow-expand-vertical";
}
int8_t UltrasonicSensorComponent::accuracy_decimals() {
  return 2; // cm precision
}

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_ULTRASONIC_SENSOR
