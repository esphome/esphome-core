//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/sensor/sensor.h"

#include "esphomelib/log.h"

#ifdef USE_SENSOR

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor.sensor";

void Sensor::add_new_value_callback(sensor_callback_t callback) {
  this->callback_.add(std::move(callback));
}

void Sensor::push_new_value(float value) {
  this->callback_.call(value);
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
int8_t Sensor::accuracy_decimals() {
  return 0;
}

PollingSensorComponent::PollingSensorComponent(uint32_t update_interval)
    : PollingComponent(update_interval) {}
uint32_t PollingSensorComponent::update_interval() {
  return this->get_update_interval();
}

} // namespace sensor

} // namespace esphomelib

#endif //USE_SENSOR
