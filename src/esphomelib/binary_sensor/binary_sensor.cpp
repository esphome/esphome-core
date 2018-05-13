//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/binary_sensor/binary_sensor.h"

#ifdef USE_BINARY_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

void BinarySensor::add_on_state_callback(binary_callback_t &&callback) {
  this->state_callback_.add(std::move(callback));
}

void BinarySensor::publish_state(bool state) {
  bool actual = state != this->inverted_;
  if (!this->first_value_ && actual == this->value_)
    return;
  this->first_value_ = false;
  this->value_ = actual;
  this->state_callback_.call(actual);
}
bool BinarySensor::is_inverted() const {
  return this->inverted_;
}
void BinarySensor::set_inverted(bool inverted) {
  this->inverted_ = inverted;
}
std::string BinarySensor::device_class() {
  return "";
}
BinarySensor::BinarySensor(const std::string &name) : Nameable(name) {}
bool BinarySensor::get_value() const {
  return this->value_;
}
void BinarySensor::set_device_class(const std::string &device_class) {
  this->device_class_ = device_class;
}
std::string BinarySensor::get_device_class() {
  if (this->device_class_.defined)
    return this->device_class_.value;
  return this->device_class();
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR
