//
// Created by Otto Winter on 26.11.17.
//

#include <utility>
#include "sensor.h"

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

std::string TemperatureSensor::unit_of_measurement() {
  return "°C";
}

std::string HumiditySensor::unit_of_measurement() {
  return "%";
}

} // namespace sensor

} // namespace esphomelib
