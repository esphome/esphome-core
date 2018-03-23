//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/sensor/sensor.h"

#include <utility>

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::sensor";

void Sensor::set_new_value_callback(sensor_callback_t callback) {
  this->callback_ = std::move(callback);
}

void Sensor::push_new_value(float value, int8_t accuracy_decimals) {
  if (this->callback_)
    this->callback_(value, accuracy_decimals);
}
uint32_t Sensor::get_update_interval() const {
  return this->update_interval_;
}
void Sensor::set_update_interval(uint32_t update_interval) {
  this->update_interval_ = update_interval;
}

Sensor::Sensor(uint32_t update_interval) : update_interval_(update_interval) {

}
std::string Sensor::unit_of_measurement() {
  return "";
}
std::string Sensor::icon() {
  return "";
}

std::string TemperatureSensor::unit_of_measurement() {
  return "°C";
}
TemperatureSensor::TemperatureSensor(uint32_t update_interval) : Sensor(update_interval) {

}
std::string TemperatureSensor::icon() {
  // Home Assistant will automatically chose the correct icon for sensors with
  // a temperature unit_of_measurement, so disable icon to save discovery payload
  // size.
  return "";
}

HumiditySensor::HumiditySensor(uint32_t update_interval) : Sensor(update_interval) { }

std::string HumiditySensor::unit_of_measurement() {
  return "%";
}
std::string HumiditySensor::icon() {
  return "mdi:water-percent";
}

VoltageSensor::VoltageSensor(uint32_t update_interval) : Sensor(update_interval) { }
std::string VoltageSensor::unit_of_measurement() {
  return "V";
}
std::string VoltageSensor::icon() {
  return "mdi:flash";
}
DistanceSensor::DistanceSensor(uint32_t update_interval) : Sensor(update_interval) { }
std::string DistanceSensor::unit_of_measurement() {
  return "m";
}
std::string DistanceSensor::icon() {
  return "mdi:arrow-expand-vertical";
}
} // namespace sensor

} // namespace esphomelib
