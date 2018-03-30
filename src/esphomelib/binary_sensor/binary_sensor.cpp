//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/binary_sensor/binary_sensor.h"

namespace esphomelib {

namespace binary_sensor {

static const char *TAG = "binary_sensor::binary_sensor";

void BinarySensor::add_on_new_state_callback(binary_callback_t &&callback) {
  this->new_state_callback_.add(std::move(callback));
}

void BinarySensor::publish_state(bool state) {
  this->new_state_callback_.call(state != this->inverted_);
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

} // namespace binary_sensor

} // namespace esphomelib
