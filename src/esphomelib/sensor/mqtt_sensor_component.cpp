//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/sensor/mqtt_sensor_component.h"

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/component.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::mqtt";

void MQTTSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT Sensor '%s'", this->friendly_name_.c_str());
  if (this->expire_after_.defined)
    ESP_LOGCONFIG(TAG, "    Expire After: %u s", this->expire_after_.value / 1000);
  ESP_LOGCONFIG(TAG, "    Unit of Measurement: '%s'", this->unit_of_measurement_.c_str());
  if (this->override_accuracy_decimals_.defined)
    ESP_LOGCONFIG(TAG, "    Override Accuracy Decimals: %i", this->override_accuracy_decimals_.value);
  if (!this->icon_.empty())
    ESP_LOGCONFIG(TAG, "    Icon: '%s'", this->icon_.c_str());
  ESP_LOGCONFIG(TAG, "    # Filters: %u", this->filters_.size());


  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
    if (!this->unit_of_measurement_.empty())
      root["unit_of_measurement"] = this->unit_of_measurement_;

    if (this->expire_after_.defined)
      root["expire_after"] = this->expire_after_.value / 1000;

    // TODO: Enable this once a new Home Assistant version is out.
    // if (!this->icon_.empty())
    //   root["icon"] = this->icon_;
  }, true, false); // enable state topic, disable command topic
}

sensor_callback_t MQTTSensorComponent::create_new_data_callback() {
  return [this](float value, int8_t accuracy_decimals) {
    // This stores the current value after each filter step.
    float current_value = value;

    ESP_LOGV(TAG, "'%s': Received new value %f with accuracy %d",
             this->friendly_name_.c_str(), value, accuracy_decimals);

    for (unsigned int i = 0; i < this->filters_.size(); i++) {
      // Apply the filter
      Filter *filter = this->filters_[i];
      auto optional_value = filter->new_value(current_value);
      if (!optional_value.defined) {
        ESP_LOGV(TAG, "'%s':  Filter #%u aborted chain",
                 this->friendly_name_.c_str(), i);
        // The filter aborted the chain
        return;
      }
      ESP_LOGV(TAG, "'%s':  Filter #%u %.2f -> %.2f",
               this->friendly_name_.c_str(), i, current_value, optional_value.value);
      current_value = optional_value.value;
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

void MQTTSensorComponent::set_expire_after(uint32_t expire_after) {
  this->expire_after_ = expire_after;
}

void MQTTSensorComponent::push_out_value(float value, int8_t accuracy_decimals) {
  ESP_LOGD(TAG, "'%s': Pushing out value %f with accuracy %d",
           this->friendly_name_.c_str(), value, accuracy_decimals);
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
  this->add_filter(new LambdaFilter(std::move(filter)));
}
void MQTTSensorComponent::add_offset_filter(float offset) {
  this->add_filter(new OffsetFilter(offset));
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

  sensor->add_new_value_callback(this->create_new_data_callback());
  // By default, smooth over the last 15 values using sliding window moving average.
  this->add_sliding_window_average_filter(15, 15);
  // By default, expire after 30 missed values, or two full missed sliding windows.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
  auto *polling = dynamic_cast<PollingObject *>(sensor);
#pragma clang diagnostic pop
  if (polling != nullptr) {
    // If this is a polling sensor
    uint32_t expire_after = polling->get_update_interval() * 30;
    this->set_expire_after(expire_after);
  }
  this->set_unit_of_measurement(sensor->unit_of_measurement());
}
void MQTTSensorComponent::add_multiply_filter(float multiplier) {
  this->add_filter(new MultiplyFilter(multiplier));
}
void MQTTSensorComponent::add_filters(const std::vector<Filter *> &filters) {
  this->filters_.insert(this->filters_.end(), filters.begin(), filters.end());
}
void MQTTSensorComponent::set_filters(const std::vector<Filter *> &filters) {
  this->filters_.clear();
  this->add_filters(filters);
}
void MQTTSensorComponent::add_filter_out_value_filter(float values_to_filter_out) {
  this->add_filter(new FilterOutValueFilter(values_to_filter_out));
}
void MQTTSensorComponent::set_icon(const std::string &icon) {
  this->icon_ = icon;
}
const std::string &MQTTSensorComponent::get_icon() const {
  return this->icon_;
}

} // namespace sensor

} // namespace esphomelib
