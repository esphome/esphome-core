#include "esphome/defines.h"

#ifdef USE_DUTY_CYCLE_SENSOR

#include "esphome/sensor/duty_cycle_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.duty_cycle";

DutyCycleSensor::DutyCycleSensor(const std::string &name, GPIOPin *pin, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), pin_(pin) {}

void DutyCycleSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Duty Cycle Sensor '%s'...", this->get_name().c_str());
  this->pin_->setup();
  this->store_.pin = this->pin_->to_isr();
  this->store_.last_level = this->pin_->digital_read();
  this->last_update_ = micros();

  this->pin_->attach_interrupt(DutyCycleSensorStore::gpio_intr, &this->store_, CHANGE);
}
void DutyCycleSensor::dump_config() {
  LOG_SENSOR("", "Duty Cycle Sensor", this);
  LOG_PIN("  Pin: ", this->pin_);
  LOG_UPDATE_INTERVAL(this);
}
void DutyCycleSensor::update() {
  const uint32_t now = micros();
  const bool level = this->store_.last_level;
  const uint32_t last_interrupt = this->store_.last_interrupt;
  uint32_t on_time = this->store_.on_time;

  if (level)
    on_time += now - last_interrupt;

  const float total_time = float(now - this->last_update_);

  const float value = (on_time / total_time) * 100.0f;
  ESP_LOGD(TAG, "'%s' Got duty cycle=%.1f%%", this->get_name().c_str(), value);
  this->publish_state(value);

  this->store_.on_time = 0;
  this->store_.last_interrupt = now;
  this->last_update_ = now;
}

std::string DutyCycleSensor::unit_of_measurement() { return "%"; }
std::string DutyCycleSensor::icon() { return "mdi:percent"; }
int8_t DutyCycleSensor::accuracy_decimals() { return 1; }
float DutyCycleSensor::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

void ICACHE_RAM_ATTR DutyCycleSensorStore::gpio_intr(DutyCycleSensorStore *arg) {
  const bool new_level = arg->pin->digital_read();
  if (new_level == arg->last_level)
    return;
  arg->last_level = new_level;
  const uint32_t now = micros();

  if (!new_level)
    arg->on_time += now - arg->last_interrupt;

  arg->last_interrupt = now;
}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_DUTY_CYCLE_SENSOR
