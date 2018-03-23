//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/sensor/sensor.h"

#include "esphomelib/log.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::sensor";

void Sensor::add_new_value_callback(sensor_callback_t callback) {
  this->callback_.add(std::move(callback));
}

void Sensor::push_new_value(float value, int8_t accuracy_decimals) {
  this->callback_.call(value, accuracy_decimals);
}
std::string Sensor::unit_of_measurement() {
  return "";
}
std::string Sensor::icon() {
  return "";
}
uint32_t Sensor::update_interval() {
  return 0;
}

PollingSensorComponent::PollingSensorComponent(uint32_t update_interval)
    : PollingComponent(update_interval) {}
uint32_t PollingSensorComponent::update_interval() {
  return this->get_update_interval();
}

} // namespace sensor

} // namespace esphomelib
