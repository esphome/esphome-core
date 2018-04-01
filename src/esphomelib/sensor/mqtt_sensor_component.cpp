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

MQTTSensorComponent::MQTTSensorComponent(const std::string &friendly_name, Sensor *sensor)
    : MQTTComponent(friendly_name) {
  assert(sensor != nullptr);
  this->sensor_ = sensor;
  this->sensor_->add_new_value_callback(this->create_new_data_callback());

  if (this->sensor_->update_interval() > 0) {
    // By default, smooth over the last 15 values using sliding window moving average.
    this->add_sliding_window_average_filter(15, 15);
  }
}

void MQTTSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT Sensor '%s'", this->friendly_name_.c_str());
  if (this->get_expire_after() > 0)
    ESP_LOGCONFIG(TAG, "    Expire After: %us", this->get_expire_after() / 1000);
  ESP_LOGCONFIG(TAG, "    Unit of Measurement: '%s'", this->get_unit_of_measurement().c_str());
  ESP_LOGCONFIG(TAG, "    Accuracy Decimals: %i", this->get_accuracy_decimals());
  ESP_LOGCONFIG(TAG, "    Icon: '%s'", this->get_icon().c_str());
  ESP_LOGCONFIG(TAG, "    # Filters: %u", this->filters_.size());

  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
    if (!this->get_unit_of_measurement().empty())
      root["unit_of_measurement"] = this->get_unit_of_measurement();

    if (this->get_expire_after() > 0)
      root["expire_after"] = this->get_expire_after() / 1000;

    if (!this->get_icon().empty())
      root["icon"] = this->get_icon();
  }, true, false); // enable state topic, disable command topic
}

sensor_callback_t MQTTSensorComponent::create_new_data_callback() {
  return [this](float value) {
    // This stores the current value after each filter step.
    float current_value = value;

    ESP_LOGV(TAG, "'%s': Received new value %f", this->friendly_name_.c_str(), value);

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

    // All filters succeeded, push out the result.
    this->push_out_value(current_value, this->get_accuracy_decimals());
  };
}

std::string MQTTSensorComponent::component_type() const {
  return "sensor";
}

void MQTTSensorComponent::push_out_value(float value, int8_t accuracy_decimals) {
  ESP_LOGD(TAG, "'%s': Pushing out value %f with accuracy %d",
           this->friendly_name_.c_str(), value, accuracy_decimals);
  auto multiplier = float(pow10(accuracy_decimals));
  float value_rounded = roundf(value * multiplier) / multiplier;
  char tmp[32]; // should be enough, but we should maybe improve this at some point.
  dtostrf(value_rounded, 0, uint8_t(std::max(0, int(accuracy_decimals))), tmp);
  this->send_message(this->get_state_topic(), tmp);
}
uint32_t MQTTSensorComponent::get_expire_after() const {
  if (this->expire_after_.defined) {
    return this->expire_after_.value;
  } else {
    // By default, expire after 50 missed values (3 sliding window cycles
    return this->sensor_->update_interval() * 45;
  }
}
std::string MQTTSensorComponent::get_unit_of_measurement() const {
  if (this->unit_of_measurement_.defined) {
    return this->unit_of_measurement_.value;
  } else {
    assert(this->sensor_ != nullptr);
    return this->sensor_->unit_of_measurement();
  }
}
int8_t MQTTSensorComponent::get_accuracy_decimals() const {
  if (this->accuracy_decimals_.defined) {
    return this->accuracy_decimals_.value;
  } else {
    assert(this->sensor_ != nullptr);
    return this->sensor_->accuracy_decimals();
  }
}
std::string MQTTSensorComponent::get_icon() const {
  if (this->icon_.defined) {
    return this->icon_.value;
  } else {
    return this->sensor_->icon();
  }
}
void MQTTSensorComponent::set_unit_of_measurement(const std::string &unit_of_measurement) {
  this->unit_of_measurement_ = unit_of_measurement;
}

void MQTTSensorComponent::set_expire_after(uint32_t expire_after) {
  this->expire_after_ = expire_after;
}
void MQTTSensorComponent::disable_expire_after() {
  this->expire_after_ = 0;
}
void MQTTSensorComponent::set_icon(const std::string &icon) {
  this->icon_ = icon;
}
void MQTTSensorComponent::set_accuracy_decimals(int8_t accuracy_decimals) {
  this->accuracy_decimals_ = accuracy_decimals;
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

} // namespace sensor

} // namespace esphomelib
