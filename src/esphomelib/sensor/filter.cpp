//
// Created by Otto Winter on 03.12.17.
//

#include "esphomelib/sensor/filter.h"

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"

#ifdef USE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

SlidingWindowMovingAverageFilter::SlidingWindowMovingAverageFilter(size_t window_size, size_t send_every)
    : send_every_(send_every), send_at_(send_every - 1),
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
  return Optional<float>();
}
uint32_t SlidingWindowMovingAverageFilter::expected_interval(uint32_t input) {
  return input * this->send_every_;
}

ExponentialMovingAverageFilter::ExponentialMovingAverageFilter(float alpha, size_t send_every)
    : send_every_(send_every), send_at_(send_every - 1),
      value_average_(ExponentialMovingAverage(alpha)),
      accuracy_average_(ExponentialMovingAverage(alpha)) {
}
Optional<float> ExponentialMovingAverageFilter::new_value(float value) {
  float average_value = this->value_average_.next_value(value);

  if (++this->send_at_ >= this->send_every_) {
    this->send_at_ = 0;
    return average_value;
  }
  return Optional<float>();
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
uint32_t ExponentialMovingAverageFilter::expected_interval(uint32_t input) {
  return input * this->send_every_;
}
LambdaFilter::LambdaFilter(lambda_filter_t lambda_filter)
    : lambda_filter_(std::move(lambda_filter)) {

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

Optional<float> OffsetFilter::new_value(float value) {
  return value + this->offset_;
}

OffsetFilter::OffsetFilter(float offset)
    : offset_(offset) { }

MultiplyFilter::MultiplyFilter(float multiplier)
    : multiplier_(multiplier) { }

Optional<float> MultiplyFilter::new_value(float value) {
  return value * this->multiplier_;
}

FilterOutValueFilter::FilterOutValueFilter(float value_to_filter_out)
    : value_to_filter_out_(value_to_filter_out) {

}
Optional<float> FilterOutValueFilter::new_value(float value) {
  if (value == this->value_to_filter_out_)
    return Optional<float>();
  return value;
}

Optional<float> FilterOutNANFilter::new_value(float value) {
  if (isnan(value))
    return Optional<float>();
  return value;
}

uint32_t Filter::expected_interval(uint32_t input) {
  return input;
}

Filter::~Filter() = default;

ThrottleFilter::ThrottleFilter(uint32_t min_time_between_updates)
    : min_time_between_updates_(min_time_between_updates), Filter() {

}
Optional<float> ThrottleFilter::new_value(float value) {
  const uint32_t now = millis();
  if (this->last_update_ == 0 || now - this->last_update_ >= min_time_between_updates_) {
    this->last_update_ = now;
    return value;
  }
  return Optional<float>();
}
DeltaFilter::DeltaFilter(float min_delta)
    : min_delta_(min_delta), last_value_(NAN) {

}
Optional<float> DeltaFilter::new_value(float value) {
  if (isnan(value))
    return Optional<float>();
  if (isnan(this->last_value_)) {
    return this->last_value_ = value;
  }
  if (fabsf(value - this->last_value_) >= this->min_delta_) {
    return this->last_value_ = value;
  }
  return Optional<float>();
}
OrFilter::OrFilter(std::list<Filter *> filters)
    : filters_(std::move(filters)) {

}
Optional<float> OrFilter::new_value(float value) {
  for (Filter *filter : this->filters_) {
    auto out = filter->new_value(value);
    if (out.defined)
      return out.value;
  }

  return Optional<float>();
}
AndFilter::AndFilter(std::list<Filter *> filters)
    : filters_(std::move(filters)) {

}
Optional<float> AndFilter::new_value(float value) {
  for (Filter *filter : this->filters_) {
    auto out = filter->new_value(value);
    if (!out.defined)
      return Optional<float>();
  }
  return value;
}

Optional<float> UniqueFilter::new_value(float value) {
  if (isnan(this->last_value_) || value != this->last_value_) {
    return this->last_value_ = value;
  }
  return Optional<float>();
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR
