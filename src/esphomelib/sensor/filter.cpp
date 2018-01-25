//
// Created by Otto Winter on 03.12.17.
//

#include "filter.h"
#include <esp_log.h>
#include <esphomelib/espmath.h>

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::filter";

void Filter::set_send_value_callback(sensor_callback_t callback) {
  this->callback_ = std::move(callback);
}

void Filter::send_value(float value, int8_t accuracy_decimals) {
  this->callback_(value, accuracy_decimals);
}

SlidingWindowMovingAverageFilter::SlidingWindowMovingAverageFilter(size_t window_size, size_t send_every)
    : send_every_(send_every), send_at_(0),
      value_average_(SlidingWindowMovingAverage<float>(window_size)),
      accuracy_average_(SlidingWindowMovingAverage<int>(window_size)) {

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
  this->accuracy_average_.set_max_size(window_size);
}
void SlidingWindowMovingAverageFilter::new_value(float value, int8_t accuracy_decimals) {
  this->value_average_.next_value(value);
  this->accuracy_average_.next_value(accuracy_decimals);

  if (++this->send_at_ >= this->send_every_) {
    this->send_at_ = 0;
    float av_value = this->value_average_.calculate_average();
    auto av_accuracy = int8_t(this->accuracy_average_.calculate_average());
    this->send_value(av_value, av_accuracy);
  }
}

ExponentialMovingAverageFilter::ExponentialMovingAverageFilter(float alpha, size_t send_every)
    : send_every_(send_every), send_at_(0),
      value_average_(ExponentialMovingAverage(alpha)),
      accuracy_average_(ExponentialMovingAverage(alpha)) {
}
void ExponentialMovingAverageFilter::new_value(float value, int8_t accuracy_decimals) {
  this->value_average_.next_value(value);
  this->accuracy_average_.next_value(accuracy_decimals);

  if (++this->send_at_ >= this->send_every_) {
    this->send_at_ = 0;
    float av_value = this->value_average_.calculate_average();
    auto av_accuracy = int8_t(lroundf(this->accuracy_average_.calculate_average()));
    this->send_value(av_value, av_accuracy);
  }
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
} // namespace sensor

} // namespace esphomelib
