//
// Created by Otto Winter on 03.12.17.
//

#include "esphomelib/defines.h"

#ifdef USE_SENSOR

#include "esphomelib/sensor/filter.h"
#include "esphomelib/sensor/sensor.h"

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

SlidingWindowMovingAverageFilter::SlidingWindowMovingAverageFilter(size_t window_size, size_t send_every)
    : send_every_(send_every), send_at_(send_every - 1),
      value_average_(SlidingWindowMovingAverage(window_size)) {

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
optional<float> SlidingWindowMovingAverageFilter::new_value(float value) {
  float average_value = this->value_average_.next_value(value);

  if (++this->send_at_ >= this->send_every_) {
    this->send_at_ = 0;
    return average_value;
  }
  return {};
}
uint32_t SlidingWindowMovingAverageFilter::expected_interval(uint32_t input) {
  return input * this->send_every_;
}

ExponentialMovingAverageFilter::ExponentialMovingAverageFilter(float alpha, size_t send_every)
    : send_every_(send_every), send_at_(send_every - 1),
      value_average_(ExponentialMovingAverage(alpha)),
      accuracy_average_(ExponentialMovingAverage(alpha)) {
}
optional<float> ExponentialMovingAverageFilter::new_value(float value) {
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
optional<float> LambdaFilter::new_value(float value) {
  return this->lambda_filter_(value);
}

optional<float> OffsetFilter::new_value(float value) {
  return value + this->offset_;
}

OffsetFilter::OffsetFilter(float offset)
    : offset_(offset) { }

MultiplyFilter::MultiplyFilter(float multiplier)
    : multiplier_(multiplier) { }

optional<float> MultiplyFilter::new_value(float value) {
  return value * this->multiplier_;
}

FilterOutValueFilter::FilterOutValueFilter(float value_to_filter_out)
    : value_to_filter_out_(value_to_filter_out) {

}
optional<float> FilterOutValueFilter::new_value(float value) {
  if (value == this->value_to_filter_out_)
    return {};
  return value;
}

optional<float> FilterOutNANFilter::new_value(float value) {
  if (isnan(value))
    return {};
  return value;
}

uint32_t Filter::expected_interval(uint32_t input) {
  return input;
}
void Filter::input(float value) {
  optional<float> out = this->new_value(value);
  if (out.has_value())
    this->output_(*out);
}
void Filter::initialize(std::function<void(float)> &&output) {
  this->output_ = std::move(output);
}

Filter::~Filter() {
  delete this->next_;
}

ThrottleFilter::ThrottleFilter(uint32_t min_time_between_inputs)
    : min_time_between_inputs_(min_time_between_inputs), Filter() {

}
optional<float> ThrottleFilter::new_value(float value) {
  const uint32_t now = millis();
  if (this->last_input_ == 0 || now - this->last_input_ >= min_time_between_inputs_) {
    this->last_input_ = now;
    return value;
  }
  this->last_input_ = now;
  return {};
}
DeltaFilter::DeltaFilter(float min_delta)
    : min_delta_(min_delta), last_value_(NAN) {

}
optional<float> DeltaFilter::new_value(float value) {
  if (isnan(value))
    return {};
  if (isnan(this->last_value_)) {
    return this->last_value_ = value;
  }
  if (fabsf(value - this->last_value_) >= this->min_delta_) {
    return this->last_value_ = value;
  }
  return {};
}
OrFilter::OrFilter(std::list<Filter *> filters)
    : filters_(std::move(filters)) {

}
optional<float> OrFilter::new_value(float value) {
  for (Filter *filter : this->filters_)
    filter->input(value);

  return {};
}
OrFilter::~OrFilter() {
  delete this->next_;
  for (Filter *filter : this->filters_) {
    delete filter;
  }
}

void OrFilter::initialize(std::function<void(float)> &&output) {
  Filter::initialize(std::move(output));
  for (Filter *filter : this->filters_) {
    filter->initialize([this](float value) {
      this->output_(value);
    });
  }
}

uint32_t OrFilter::expected_interval(uint32_t input) {
  uint32_t min_interval = UINT32_MAX;
  for (Filter *filter : this->filters_) {
    min_interval = std::min(min_interval, filter->expected_interval(input));
  }

  return min_interval;
}

optional<float> UniqueFilter::new_value(float value) {
  if (isnan(this->last_value_) || value != this->last_value_) {
    return this->last_value_ = value;
  }

  return {};
}

optional<float> DebounceFilter::new_value(float value) {
  this->set_timeout("debounce", this->time_period_, [this, value](){
    this->output_(value);
  });

  return {};
}

DebounceFilter::DebounceFilter(uint32_t time_period)
    : time_period_(time_period) {

}

HeartbeatFilter::HeartbeatFilter(uint32_t time_period)
    : time_period_(time_period), last_input_(NAN) {

}

optional<float> HeartbeatFilter::new_value(float value) {
  this->last_input_ = value;

  return {};
}
uint32_t HeartbeatFilter::expected_interval(uint32_t input) {
  return this->time_period_;
}
void HeartbeatFilter::setup() {
  this->set_interval("heartbeat", this->time_period_, [this]() {
    this->output_(this->last_input_);
  });
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR
