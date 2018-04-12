//
// Created by Otto Winter on 03.12.17.
//

#ifndef ESPHOMELIB_SENSOR_FILTER_H
#define ESPHOMELIB_SENSOR_FILTER_H

#include <cstdint>

#include "esphomelib/helpers.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_SENSOR

namespace esphomelib {

namespace sensor {

/** Apply a filter to sensor values such as moving average.
 *
 * This class is purposefully kept quite simple, since more complicated
 * filters should really be done with the filter sensor in Home Assistant.
 */
class Filter {
 public:
  /** This will be called every time the filter receives a new value.
   *
   * It can return an empty optional to indicate that the filter chain
   * should stop, otherwise the value in the filter will be passed down
   * the chain.
   *
   * @param value The new value.
   * @return An optional float, the new value that should be pushed out.
   */
  virtual Optional<float> new_value(float value) = 0;
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
  explicit SlidingWindowMovingAverageFilter(size_t window_size, size_t send_every);

  Optional<float> new_value(float value) override;

  size_t get_send_every() const;
  void set_send_every(size_t send_every);
  size_t get_window_size() const;
  void set_window_size(size_t window_size);

 protected:
  SlidingWindowMovingAverage<float> value_average_;
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

  Optional<float> new_value(float value) override;

  size_t get_send_every() const;
  void set_send_every(size_t send_every);
  float get_alpha() const;
  void set_alpha(float alpha);

 protected:
  ExponentialMovingAverage value_average_;
  ExponentialMovingAverage accuracy_average_;
  size_t send_every_;
  size_t send_at_;
};

using lambda_filter_t = std::function<Optional<float>(float)>;

/** This class allows for creation of simple template filters.
 *
 * The constructor accepts a lambda of the form float -> Optional<float>.
 * It will be called with each new value in the filter chain and returns the modified
 * value that shall be passed down the filter chain. Returning an empty Optional
 * means that the value shall be discarded.
 */
class LambdaFilter : public Filter {
 public:
  explicit LambdaFilter(lambda_filter_t lambda_filter);

  Optional<float> new_value(float value) override;

  const lambda_filter_t &get_lambda_filter() const;
  void set_lambda_filter(const lambda_filter_t &lambda_filter);
 protected:
  lambda_filter_t lambda_filter_;
};

/// A simple filter that adds `offset` to each value it receives.
class OffsetFilter : public Filter {
 public:
  explicit OffsetFilter(float offset);

  Optional<float> new_value(float value) override;

 protected:
  float offset_;
};

/// A simple filter that multiplies to each value it receives by `multiplier`.
class MultiplyFilter : public Filter {
 public:
  explicit MultiplyFilter(float multiplier);

  Optional<float> new_value(float value) override;

 protected:
  float multiplier_;
};

/// A simple filter that only forwards the filter chain if it doesn't receive `value_to_filter_out`.
class FilterOutValueFilter : public Filter {
 public:
  explicit FilterOutValueFilter(float values_to_filter_out);

  Optional<float> new_value(float value) override;

 protected:
  float value_to_filter_out_;
};

/// A simple filter that only forwards the filter chain if it doesn't receive `nan`.
class FilterOutNANFilter : public Filter {
 public:
  Optional<float> new_value(float value) override;
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_SENSOR

#endif //ESPHOMELIB_SENSOR_FILTER_H
