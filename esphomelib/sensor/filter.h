//
// Created by Otto Winter on 03.12.17.
//

#ifndef ESPHOMELIB_SENSOR_FILTER_H
#define ESPHOMELIB_SENSOR_FILTER_H

#include <cstdint>
#include <esphomelib/helpers.h>
#include "sensor.h"

namespace esphomelib {

namespace sensor {

/// Filter - Apply a filter to sensor values such as moving average.
class Filter {
 public:
  /// This will be called every time a new sensor value is received.
  virtual void new_value(float value, int8_t accuracy_decimals) = 0;

  /// Set the callback that will be called when the filter decides to push out data.
  void set_send_value_callback(sensor_callback_t callback);

  /// Push out a value to the front-end.
  void send_value(float value, int8_t accuracy_decimals);

 private:
  sensor_callback_t callback_;
};

/** Simple sliding window moving average filter.
 *
 * Essentially just takes takes the average of the last window_size values and pushes them out
 * every send_every.
 */
class SlidingWindowMovingAverageFilter : public Filter {
 public:
  /** Construct a SlidingWindowMovingAverageFilter.
   *
   * @param window_size The number of values that should be averaged.
   * @param send_every After how many sensor values should a new one be pushed out.
   */
  SlidingWindowMovingAverageFilter(size_t window_size, size_t send_every);

  void new_value(float value, int8_t accuracy_decimals) override;

  size_t get_send_every() const;
  void set_send_every(size_t send_every);
  size_t get_window_size() const;
  void set_window_size(size_t window_size);

 private:
  SlidingWindowMovingAverage<float> value_average_;
  SlidingWindowMovingAverage<int> accuracy_average_;
  size_t send_every_;
  size_t send_at_;
};

/** Simple exponential moving average filter.
 *
 * Essentially just takes the average of the last few values using exponentially decaying weights.
 * Use alpha to adjust decay rate.
 */
class ExponentialMovingAverageFilter : public Filter {
 public:
  ExponentialMovingAverageFilter(float alpha, size_t send_every);

  void new_value(float value, int8_t accuracy_decimals) override;

  size_t get_send_every() const;
  void set_send_every(size_t send_every);
  float get_alpha() const;
  void set_alpha(float alpha);

 private:
  ExponentialMovingAverage value_average_;
  ExponentialMovingAverage accuracy_average_;
  size_t send_every_;
  size_t send_at_;
};

} // namespace sensor

} // namespace esphomelib

#endif //ESPHOMELIB_SENSOR_FILTER_H
