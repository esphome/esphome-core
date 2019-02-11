#include "esphome/defines.h"

#ifdef USE_DUTY_CYCLE_SENSOR

#include "esphome/sensor/duty_cycle_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.duty_cycle";

DutyCycleSensor::DutyCycleSensor(const std::string &name, GPIOPin *pin, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), pin_(pin) {

}

DutyCycleSensor *duty_cycle_sensors = nullptr;

void ICACHE_RAM_ATTR HOT DutyCycleSensor::gpio_intr() {
  DutyCycleSensor *sensor = duty_cycle_sensors;
  while (sensor != nullptr) {
    sensor->on_interrupt();
    sensor = sensor->next_;
  }
}
void DutyCycleSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Duty Cycle Sensor '%s'...", this->get_name().c_str());
  this->pin_->setup();
  this->last_level_ = this->pin_->digital_read();

  disable_interrupts();
  if (duty_cycle_sensors == nullptr) {
    duty_cycle_sensors = this;
  } else {
    DutyCycleSensor *last_sensor = duty_cycle_sensors;
    while (last_sensor->next_ != nullptr) {
      last_sensor = last_sensor->next_;
    }
    last_sensor->next_ = this;
  }
  enable_interrupts();

  attachInterrupt(this->pin_->get_pin(), gpio_intr, CHANGE);
}
void DutyCycleSensor::dump_config() {
  LOG_SENSOR("", "Duty Cycle Sensor", this);
  LOG_PIN("  Pin: ", this->pin_);
  LOG_UPDATE_INTERVAL(this);
}
void ICACHE_RAM_ATTR HOT DutyCycleSensor::on_interrupt() {
  const bool new_level = this->pin_->digital_read();
  if (new_level == this->last_level_)
    return;
  this->last_level_ = new_level;
  const uint32_t now = micros();

  if (!new_level)
    this->on_time_ += now - this->last_interrupt_;

  this->last_interrupt_ = now;
}
void DutyCycleSensor::update() {
  const uint32_t now = micros();
  const bool level = this->last_level_;
  const uint32_t last_interrupt = this->last_interrupt_;
  uint32_t on_time = this->on_time_;

  if (level)
    on_time += now - last_interrupt;

  const float total_time = this->get_update_interval() * 1000.0f;

  const float value = (on_time / total_time) * 100.0f;
  ESP_LOGD(TAG, "'%s' Got duty cycle=%.1f%%", this->get_name().c_str(), value);
  this->publish_state(value);

  this->on_time_ = 0;
  this->last_interrupt_ = now;
}

std::string DutyCycleSensor::unit_of_measurement() {
  return "%";
}
std::string DutyCycleSensor::icon() {
  return "mdi:percent";
}
int8_t DutyCycleSensor::accuracy_decimals() {
  return 1;
}
float DutyCycleSensor::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_DUTY_CYCLE_SENSOR
