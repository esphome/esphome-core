//
// Created by Otto Winter on 03.12.17.
//

#include "esphomelib/sensor/filter.h"

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::filter";

SlidingWindowMovingAverageFilter::SlidingWindowMovingAverageFilter(size_t window_size, size_t send_every)
    : send_every_(send_every), send_at_(0),
      value_average_(SlidingWindowMovingAverage<float>(window_size)) {

}
size_t SlidingWindowMovingAverageFilter::get_send_every() const {
  return this->send_every_;
}
void SlidingWindowMovingAverageFilter::set_send_every(size_t send_every) {
  this->send_every_ = send_every;
}
size_t SlidingWindowMovingAverageFilter::get_window_size() const {
  return this->value_average_.get_max_size();
}
void SlidingWindowMovingAverageFilter::set_window_size(size_t window_size) {
  this->value_average_.set_max_size(window_size);
}
Optional<float> SlidingWindowMovingAverageFilter::new_value(float value) {
  float average_value = this->value_average_.next_value(value);

  if (++this->send_at_ >= this->send_every_) {
    this->send_at_ = 0;
    return average_value;
  }
  return {};
}

ExponentialMovingAverageFilter::ExponentialMovingAverageFilter(float alpha, size_t send_every)
    : send_every_(send_every), send_at_(0),
      value_average_(ExponentialMovingAverage(alpha)),
      accuracy_average_(ExponentialMovingAverage(alpha)) {
}
Optional<float> ExponentialMovingAverageFilter::new_value(float value) {
  float average_value = this->value_average_.next_value(value);

  if (++this->send_at_ >= this->send_every_) {
    this->send_at_ = 0;
    return average_value;
  }
  return {};
}
size_t ExponentialMovingAverageFilter::get_send_every() const {
  return this->send_every_;
}
void ExponentialMovingAverageFilter::set_send_every(size_t send_every) {
  this->send_every_ = send_every;
}
float ExponentialMovingAverageFilter::get_alpha() const {
  return this->value_average_.get_alpha();
}
void ExponentialMovingAverageFilter::set_alpha(float alpha) {
  this->value_average_.set_alpha(alpha);
  this->accuracy_average_.set_alpha(alpha);
}
LambdaFilter::LambdaFilter(const lambda_filter_t &lambda_filter)
    : lambda_filter_(lambda_filter) {

}
const lambda_filter_t &LambdaFilter::get_lambda_filter() const {
  return this->lambda_filter_;
}
void LambdaFilter::set_lambda_filter(const lambda_filter_t &lambda_filter) {
  this->lambda_filter_ = lambda_filter;
}
Optional<float> LambdaFilter::new_value(float value) {
  return this->lambda_filter_(value);
}

} // namespace sensor

} // namespace esphomelib
