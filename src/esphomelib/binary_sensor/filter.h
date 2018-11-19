#ifndef ESPHOMELIB_FILTER_H
#define ESPHOMELIB_FILTER_H

#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphomelib/binary_sensor/binary_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

class BinarySensor;

class Filter {
 public:
  virtual optional<bool> new_value(bool value) = 0;

  void input(bool value);

  void output(bool value);

 protected:
  friend BinarySensor;

  Filter *next_{nullptr};
  BinarySensor *parent_{nullptr};
};

class DelayedOnFilter : public Filter, public Component {
 public:
  explicit DelayedOnFilter(uint32_t delay);

  optional<bool> new_value(bool value) override;

  float get_setup_priority() const override;

 protected:
  uint32_t delay_;
};

class DelayedOffFilter : public Filter, public Component {
 public:
  explicit DelayedOffFilter(uint32_t delay);

  optional<bool> new_value(bool value) override;

  float get_setup_priority() const override;

 protected:
  uint32_t delay_;
};

class HeartbeatFilter : public Filter, public Component {
 public:
  explicit HeartbeatFilter(uint32_t interval);

  optional<bool> new_value(bool value) override;

  void setup();

  float get_setup_priority() const override;

 protected:
  uint32_t interval_;
  optional<bool> value_{};
};

class InvertFilter : public Filter {
 public:
  optional<bool> new_value(bool value) override;
};

class LambdaFilter : public Filter {
 public:
  explicit LambdaFilter(const std::function<optional<bool>(bool)> &f);

  optional<bool> new_value(bool value) override;

 protected:
  std::function<optional<bool>(bool)> f_;
};

class UniqueFilter : public Filter {
 public:
  optional<bool> new_value(bool value) override;

 protected:
  optional<bool> last_value_{};
};

}  // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_BINARY_SENSOR

#endif  // ESPHOMELIB_FILTER_H
