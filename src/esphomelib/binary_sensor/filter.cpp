#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphomelib/binary_sensor/filter.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

void Filter::output(bool value, bool is_initial) {
  if (this->next_ == nullptr) {
    this->parent_->send_state_internal_(value, is_initial);
  } else {
    this->next_->input(value, is_initial);
  }
}
void Filter::input(bool value, bool is_initial) {
  auto b = this->new_value(value, is_initial);
  if (b.has_value()) {
    this->output(*b, is_initial);
  }
}
DelayedOnFilter::DelayedOnFilter(uint32_t delay) : delay_(delay) {

}
optional<bool> DelayedOnFilter::new_value(bool value, bool is_initial) {
  if (value) {
    this->set_timeout("ON", this->delay_, [this, is_initial](){
      if (this->last_out_.value_or(false)) {
        // already sent ON
        return;
      }
      this->last_out_ = true;
      this->output(true, is_initial);
    });
    return {};
  } else {
    this->cancel_timeout("ON");
    if (!this->last_out_.value_or(true)) {
      // already sent OFF
      return {};
    }
    this->last_out_ = false;
    return false;
  }
}

float DelayedOnFilter::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

DelayedOffFilter::DelayedOffFilter(uint32_t delay) : delay_(delay) {

}
optional<bool> DelayedOffFilter::new_value(bool value, bool is_initial) {
  if (!value) {
    this->set_timeout("OFF", this->delay_, [this, is_initial](){
      if (!this->last_out_.value_or(true)) {
        // already sent OFF
        return;
      }
      this->output(false, is_initial);
    });
    return {};
  } else {
    this->cancel_timeout("OFF");
    if (this->last_out_.value_or(false)) {
      // already sent ON
      return {};
    }
    this->last_out_ = true;
    return true;
  }
}
float DelayedOffFilter::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

optional<bool> InvertFilter::new_value(bool value, bool is_initial) {
  return !value;
}

LambdaFilter::LambdaFilter(const std::function<optional<bool>(bool)> &f) : f_(f) {}

optional<bool> LambdaFilter::new_value(bool value, bool is_initial) {
  return this->f_(value);
}

optional<bool> UniqueFilter::new_value(bool value, bool is_initial) {
  if (this->last_value_.has_value() && *this->last_value_ == value) {
    return {};
  } else {
    this->last_value_ = value;
    return value;
  }
}

HeartbeatFilter::HeartbeatFilter(uint32_t interval) : interval_(interval) {}
optional<bool> HeartbeatFilter::new_value(bool value, bool is_initial) {
  this->value_ = value;
  return value;
}
void HeartbeatFilter::setup() {
  this->set_interval(this->interval_, [this]() {
    if (this->value_.has_value())
      this->output(*this->value_, false);
  });
}
float HeartbeatFilter::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR
