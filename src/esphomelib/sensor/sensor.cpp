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

std::string TemperatureSensor::unit_of_measurement() {
  return "°C";
}
std::string TemperatureSensor::icon() {
  // Home Assistant will automatically chose the correct icon for sensors with
  // a temperature unit_of_measurement, so disable icon to save discovery payload
  // size.
  return "";
}

std::string HumiditySensor::unit_of_measurement() {
  return "%";
}
std::string HumiditySensor::icon() {
  return "mdi:water-percent";
}

std::string VoltageSensor::unit_of_measurement() {
  return "V";
}
std::string VoltageSensor::icon() {
  return "mdi:flash";
}
std::string DistanceSensor::unit_of_measurement() {
  return "m";
}
std::string DistanceSensor::icon() {
  return "mdi:arrow-expand-vertical";
}

} // namespace sensor

} // namespace esphomelib
