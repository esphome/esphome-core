//
// Created by Otto Winter on 26.11.17.
//

#include "mqtt_sensor_component.h"
#include "esphomelib/espmath.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::mqtt_sensor";

MQTTSensorComponent::MQTTSensorComponent(std::string friendly_name,
                                         std::string unit_of_measurement,
                                         Optional<uint32_t> expire_after)
    : MQTTComponent(std::move(friendly_name)), unit_of_measurement_(std::move(unit_of_measurement)),
      expire_after_(expire_after), offset_(0.0f) {

}

void MQTTSensorComponent::setup() {
  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
    root["unit_of_measurement"] = this->unit_of_measurement_.c_str();
    if (this->expire_after_)
      root["expire_after"] = this->expire_after_.value;
  }, true, false);
}

sensor_callback_t MQTTSensorComponent::create_new_data_callback() {
  return [this](float value, int8_t accuracy_decimals) {
    value = value + this->offset_;
    if (this->filter_ != nullptr) {
      this->filter_->new_value(value, accuracy_decimals);
    } else {
      this->push_out_value(value, accuracy_decimals);
    }
  };
}

std::string MQTTSensorComponent::component_type() const {
  return "sensor";
}

void MQTTSensorComponent::set_unit_of_measurement(const std::string &unit_of_measurement) {
  this->unit_of_measurement_ = unit_of_measurement;
}

void MQTTSensorComponent::set_expire_after(const Optional<uint32_t> &expire_after) {
  this->expire_after_ = expire_after;
}

void MQTTSensorComponent::set_filter(Filter *filter) {
  this->filter_ = filter;
  this->filter_->set_send_value_callback([&](float value, int8_t accuracy_decimals) {
    this->push_out_value(value, accuracy_decimals);
  });
}

void MQTTSensorComponent::push_out_value(float value, int8_t accuracy_decimals) {
  auto multiplier = float(pow10(accuracy_decimals));
  float value_rounded = roundf(value * multiplier) / multiplier;
  char tmp[32];
  dtostrf(value_rounded, 0, uint8_t(std::max(0, int(accuracy_decimals))), tmp);
  this->send_message(this->get_state_topic(), tmp);
}
float MQTTSensorComponent::get_offset() const {
  return this->offset_;
}
void MQTTSensorComponent::set_offset(float offset) {
  this->offset_ = offset;
}
const std::string &MQTTSensorComponent::get_unit_of_measurement() const {
  return this->unit_of_measurement_;
}
const Optional<uint32_t> &MQTTSensorComponent::get_expire_after() const {
  return this->expire_after_;
}
void MQTTSensorComponent::disable_expiry() {
  this->expire_after_ = Optional<uint32_t>();
}
Filter *MQTTSensorComponent::get_filter() const {
  return this->filter_;
}
void MQTTSensorComponent::disable_filter() {
  this->filter_ = nullptr;
}

} // namespace sensor

} // namespace esphomelib
