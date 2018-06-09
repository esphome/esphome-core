//
//  duty_cycle_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 09.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/duty_cycle_sensor.h"
#include "esphomelib/log.h"

#ifdef USE_DUTY_CYCLE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.duty_cycle";

DutyCycleSensor::DutyCycleSensor(const std::string &name, GPIOPin *pin, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), pin_(pin) {

}

std::vector<DutyCycleSensor *> duty_cycle_sensors; // NOLINT

void gpio_intr() {
  for (auto *sensor : duty_cycle_sensors)
    sensor->on_interrupt();
}
void DutyCycleSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Duty Cycle Sensor '%s'...", this->get_name().c_str());
  this->pin_->setup();
  this->last_level_ = this->pin_->digital_read();
  duty_cycle_sensors.push_back(this);
  attachInterrupt(this->pin_->get_pin(), gpio_intr, CHANGE);
}
void DutyCycleSensor::on_interrupt() {
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

  this->on_time_ = 0;
  this->last_interrupt_ = now;
}

std::string DutyCycleSensor::unit_of_measurement() {
  return Sensor::unit_of_measurement();
}
std::string DutyCycleSensor::icon() {
  return "mdi:percent";
}
int8_t DutyCycleSensor::accuracy_decimals() {
  return 1;
}


} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_DUTY_CYCLE_SENSOR
