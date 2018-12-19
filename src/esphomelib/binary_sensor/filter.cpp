#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphomelib/binary_sensor/filter.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

void Filter::output(bool value) {
  if (this->next_ == nullptr) {
    this->parent_->send_state_internal_(value);
  } else {
    this->next_->input(value);
  }
}
void Filter::input(bool value) {
  auto b = this->new_value(value);
  if (b.has_value()) {
    this->output(*b);
  }
}
DelayedOnFilter::DelayedOnFilter(uint32_t delay) : delay_(delay) {

}
optional<bool> DelayedOnFilter::new_value(bool value) {
  if (value) {
    this->set_timeout("ON", this->delay_, [this](){
      this->output(true);
    });
    return {};
  } else {
    this->cancel_timeout("ON");
    return false;
  }
}

float DelayedOnFilter::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

DelayedOffFilter::DelayedOffFilter(uint32_t delay) : delay_(delay) {

}
optional<bool> DelayedOffFilter::new_value(bool value) {
  if (!value) {
    this->set_timeout("OFF", this->delay_, [this](){
      this->output(false);
    });
    return {};
  } else {
    this->cancel_timeout("OFF");
    return true;
  }
}
float DelayedOffFilter::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

optional<bool> InvertFilter::new_value(bool value) {
  return !value;
}

LambdaFilter::LambdaFilter(const std::function<optional<bool>(bool)> &f) : f_(f) {}

optional<bool> LambdaFilter::new_value(bool value) {
  return this->f_(value);
}

optional<bool> UniqueFilter::new_value(bool value) {
  if (this->last_value_.has_value() && *this->last_value_ == value) {
    return {};
  } else {
    this->last_value_ = value;
    return value;
  }
}

HeartbeatFilter::HeartbeatFilter(uint32_t interval) : interval_(interval) {}
optional<bool> HeartbeatFilter::new_value(bool value) {
  this->value_ = value;
  return value;
}
void HeartbeatFilter::setup() {
  this->set_interval(this->interval_, [this]() {
    if (this->value_.has_value())
      this->output(*this->value_);
  });
}
float HeartbeatFilter::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR
