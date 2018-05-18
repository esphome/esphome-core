//
// Created by Otto Winter on 26.11.17.
//

#include <utility>
#include "esphomelib/sensor/sensor.h"

#include "esphomelib/log.h"

#ifdef USE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.sensor";

void Sensor::push_new_value(float value) {
  this->raw_value_ = value;
  this->raw_callback_.call(value);

  ESP_LOGV(TAG, "'%s': Received new value %f", this->name_.c_str(), value);

  if (this->filter_list_ == nullptr) {
    this->send_value_to_frontend(value);
  } else {
    this->filter_list_->input(value);
  }
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
Sensor::Sensor(const std::string &name)
    : Nameable(name) {
  // By default, apply a smoothing over the last 15 values
  this->add_sliding_window_average_filter(15, 15);
}

void Sensor::set_unit_of_measurement(const std::string &unit_of_measurement) {
  this->unit_of_measurement_ = unit_of_measurement;
}
void Sensor::set_icon(const std::string &icon) {
  this->icon_ = icon;
}
void Sensor::set_accuracy_decimals(int8_t accuracy_decimals) {
  this->accuracy_decimals_ = accuracy_decimals;
}
void Sensor::add_on_value_callback(sensor_callback_t callback) {
  this->callback_.add(std::move(callback));
}
void Sensor::add_on_raw_value_callback(sensor_callback_t callback) {
  this->raw_callback_.add(std::move(callback));
}
std::string Sensor::get_icon() {
  if (this->icon_)
    return this->icon_.value;
  return this->icon();
}
std::string Sensor::get_unit_of_measurement() {
  if (this->unit_of_measurement_)
    return this->unit_of_measurement_.value;
  return this->unit_of_measurement();
}
int8_t Sensor::get_accuracy_decimals() {
  if (this->accuracy_decimals_)
    return this->accuracy_decimals_.value;
  return this->accuracy_decimals();
}
void Sensor::add_filter(Filter *filter) {
  // inefficient, but only happens once on every sensor setup and nobody's going to have massive amounts of
  // filters
  if (this->filter_list_ == nullptr) {
    this->filter_list_ = filter;
  } else {
    Filter *last_filter = this->filter_list_;
    while (last_filter->next_ != nullptr)
      last_filter = last_filter->next_;
    last_filter->next_ = filter;
    last_filter->output_ = [filter] (float value) {
      filter->input(value);
    };
  }
  filter->initialize([this](float value) {
    this->send_value_to_frontend(value);
  });
}
void Sensor::add_filters(const std::list<Filter *> &filters) {
  for (Filter *filter : filters) {
    this->add_filter(filter);
  }
}
void Sensor::set_filters(const std::list<Filter *> &filters) {
  this->clear_filters();
  this->add_filters(filters);
}
void Sensor::add_lambda_filter(lambda_filter_t filter) {
  this->add_filter(new LambdaFilter(std::move(filter)));
}
void Sensor::add_offset_filter(float offset) {
  this->add_filter(new OffsetFilter(offset));
}
void Sensor::add_multiply_filter(float multiplier) {
  this->add_filter(new MultiplyFilter(multiplier));
}
void Sensor::add_filter_out_value_filter(float values_to_filter_out) {
  this->add_filter(new FilterOutValueFilter(values_to_filter_out));
}
void Sensor::add_sliding_window_average_filter(size_t window_size, size_t send_every) {
  this->add_filter(new SlidingWindowMovingAverageFilter(window_size, send_every));
}
void Sensor::add_exponential_moving_average_filter(float alpha, size_t send_every) {
  this->add_filter(new ExponentialMovingAverageFilter(alpha, send_every));
}
void Sensor::clear_filters() {
  Filter *filter = this->filter_list_;
  while (filter != nullptr) {
    Filter *next_filter = filter->next_;
    delete filter;
    filter = next_filter;
  }

  this->filter_list_ = nullptr;
}
float Sensor::get_value() const {
  return this->value_;
}
float Sensor::get_raw_value() const {
  return this->raw_value_;
}
std::string Sensor::unique_id() { return ""; }

void Sensor::send_value_to_frontend(float value) {
  this->value_ = value;
  this->callback_.call(value);
}

PollingSensorComponent::PollingSensorComponent(const std::string &name, uint32_t update_interval)
    : PollingComponent(update_interval), Sensor(name) {}

uint32_t PollingSensorComponent::update_interval() {
  return this->get_update_interval();
}

const char ICON_FLASH[] = "mdi:flash";
const char UNIT_V[] = "V";
const char ICON_EMPTY[] = "";
const char UNIT_C[] = "°C";
const char ICON_GAUGE[] = "mdi:gauge";
const char UNIT_HPA[] = "hPa";
const char ICON_WATER_PERCENT[] = "mdi:water-percent";
const char UNIT_PERCENT[] = "%";
const char ICON_SCREEN_ROTATION[] = "mdi:screen-rotation";
const char ICON_BRIEFCASE_DOWNLOAD[] = "mdi:briefcase-download";
const char UNIT_DEGREES_PER_SECOND[] = "°/s";
const char UNIT_M_PER_S_SQUARED[] = "m/s²";
const char ICON_BRIGHTNESS_5[] = "mdi:brightness-5";
const char UNIT_LX[] = "lx";
const char UNIT_OHM[] = "Ω";
const char ICON_GAS_CYLINDER[] = "mdi:gas-cylinder";

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR
