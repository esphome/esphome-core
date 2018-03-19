//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/sensor/mqtt_sensor_component.h"

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/component.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::mqtt_sensor";

void MQTTSensorComponent::setup() {
  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
    root["unit_of_measurement"] = buffer.strdup(this->unit_of_measurement_.c_str());
    assert((this->expire_after_ == 0 || this->expire_after_ > 1000) &&
        "Expire after is less than a second; make sure you're providing the value in ms.");
    if (this->expire_after_)
      root["expire_after"] = this->expire_after_.value / 1000;
  }, true, false);
}

sensor_callback_t MQTTSensorComponent::create_new_data_callback() {
  return [this](float value, int8_t accuracy_decimals) {
    // This stores the current value after each filter step.
    float current_value = value;

    for (auto *filter : this->filters_) {
      // Apply the filter
      auto optional_value = filter->new_value(current_value);
      if (!optional_value.defined) {
        // The filter aborted the chain
        return;
      }
      current_value = optional_value;
    }

    if (this->override_accuracy_decimals_.defined)
      // Override accuracy_decimals if we were told to
      accuracy_decimals = this->override_accuracy_decimals_;

    // All filters succeeded, push out the result.
    this->push_out_value(current_value, accuracy_decimals);
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

void MQTTSensorComponent::push_out_value(float value, int8_t accuracy_decimals) {
  auto multiplier = float(pow10(accuracy_decimals));
  float value_rounded = roundf(value * multiplier) / multiplier;
  char tmp[32];
  dtostrf(value_rounded, 0, uint8_t(std::max(0, int(accuracy_decimals))), tmp);
  this->send_message(this->get_state_topic(), tmp);
}
const std::string &MQTTSensorComponent::get_unit_of_measurement() const {
  return this->unit_of_measurement_;
}
const Optional<uint32_t> &MQTTSensorComponent::get_expire_after() const {
  return this->expire_after_;
}
void MQTTSensorComponent::disable_expire_after() {
  this->expire_after_ = Optional<uint32_t>();
}
std::vector<Filter *> MQTTSensorComponent::get_filters() const {
  return this->filters_;
}
void MQTTSensorComponent::add_filter(Filter *filter) {
  this->filters_.push_back(filter);
}
void MQTTSensorComponent::clear_filters() {
  this->filters_.clear();
}
const Optional<int8_t> &MQTTSensorComponent::get_override_accuracy_decimals() const {
  return override_accuracy_decimals_;
}
void MQTTSensorComponent::override_accuracy_decimals(int8_t override_accuracy_decimals) {
  this->override_accuracy_decimals_ = override_accuracy_decimals;
}
void MQTTSensorComponent::add_lambda_filter(lambda_filter_t filter) {
  this->add_filter(new LambdaFilter(filter));
}
void MQTTSensorComponent::add_offset_filter(float offset) {
  this->add_lambda_filter([&](float value) -> Optional<float> {
    return value + offset;
  });
}
void MQTTSensorComponent::add_sliding_window_average_filter(size_t window_size, size_t send_every) {
  this->add_filter(new SlidingWindowMovingAverageFilter(window_size, send_every));
}
void MQTTSensorComponent::add_exponential_moving_average_filter(float alpha, size_t send_every) {
  this->add_filter(new ExponentialMovingAverageFilter(alpha, send_every));
}
MQTTSensorComponent::MQTTSensorComponent(std::string friendly_name, Sensor *sensor)
    : MQTTComponent(std::move(friendly_name)) {
  if (sensor == nullptr)
    // Disable automatic initialization
    return;

  sensor->set_new_value_callback(this->create_new_data_callback());
  // By default, smooth over the last 15 values using sliding window moving average.
  this->add_sliding_window_average_filter(15, 15);
  // By default, expire after 30 missed values, or two full missed sliding windows.
  uint32_t expire_after = sensor->get_update_interval() * 30;
  this->set_expire_after(expire_after);
}

} // namespace sensor

} // namespace esphomelib
