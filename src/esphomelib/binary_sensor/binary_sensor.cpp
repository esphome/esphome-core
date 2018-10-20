#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

void BinarySensor::add_on_state_callback(std::function<void(bool)> &&callback) {
  this->state_callback_.add(std::move(callback));
}

void BinarySensor::publish_state(bool state) {
  if (this->filter_list_ == nullptr) {
    this->send_state_internal_(state);
  } else {
    this->filter_list_->input(state);
  }

}
void BinarySensor::send_state_internal_(bool state) {
  this->has_state_ = true;
  this->state = state;
  this->state_callback_.call(state);
}
std::string BinarySensor::device_class() {
  return "";
}
BinarySensor::BinarySensor(const std::string &name)
  : Nameable(name), state(false) {

}
void BinarySensor::set_device_class(const std::string &device_class) {
  this->device_class_ = device_class;
}
std::string BinarySensor::get_device_class() {
  if (this->device_class_.has_value())
    return *this->device_class_;
  return this->device_class();
}
PressTrigger *BinarySensor::make_press_trigger() {
  return new PressTrigger(this);
}
ReleaseTrigger *BinarySensor::make_release_trigger() {
  return new ReleaseTrigger(this);
}
ClickTrigger *BinarySensor::make_click_trigger(uint32_t min_length, uint32_t max_length) {
  return new ClickTrigger(this, min_length, max_length);
}
DoubleClickTrigger *BinarySensor::make_double_click_trigger(uint32_t min_length, uint32_t max_length) {
  return new DoubleClickTrigger(this, min_length, max_length);
}
void BinarySensor::add_filter(Filter *filter) {
  filter->parent_ = this;
  if (this->filter_list_ == nullptr) {
    this->filter_list_ = filter;
  } else {
    Filter *last_filter = this->filter_list_;
    while (last_filter->next_ != nullptr)
      last_filter = last_filter->next_;
    last_filter->next_ = filter;
  }
}
void BinarySensor::add_filters(std::vector<Filter *> filters) {
  for (Filter *filter : filters) {
    this->add_filter(filter);
  }
}
bool BinarySensor::has_state() const {
  return this->has_state_;
}

PressTrigger::PressTrigger(BinarySensor *parent) {
  parent->add_on_state_callback([this](bool state) {
    if (state)
      this->trigger();
  });
}

ReleaseTrigger::ReleaseTrigger(BinarySensor *parent) {
  parent->add_on_state_callback([this](bool state) {
    if (!state)
      this->trigger();
  });
}

bool match_interval(uint32_t min_length, uint32_t max_length, uint32_t length) {
  if (max_length == 0) {
    return length >= min_length;
  } else {
    return length >= min_length && length <= max_length;
  }
}

ClickTrigger::ClickTrigger(BinarySensor *parent, uint32_t min_length, uint32_t max_length)
    : min_length_(min_length), max_length_(max_length) {
  parent->add_on_state_callback([this](bool state) {
    if (state) {
      this->start_time_ = millis();
    } else {
      const uint32_t length = millis() - this->start_time_;
      if (match_interval(this->min_length_, this->max_length_, length))
        this->trigger();
    }
  });
}

DoubleClickTrigger::DoubleClickTrigger(BinarySensor *parent, uint32_t min_length, uint32_t max_length)
    : min_length_(min_length), max_length_(max_length) {
  parent->add_on_state_callback([this](bool state) {
    const uint32_t now = millis();

    if (state && this->start_time_ != 0 && this->end_time_ != 0) {
      if (match_interval(this->min_length_, this->max_length_, this->end_time_ - this->start_time_) &&
          match_interval(this->min_length_, this->max_length_, now - this->end_time_)) {
        this->trigger();
        this->start_time_ = 0;
        this->end_time_ = 0;
        return;
      }
    }

    this->start_time_ = this->end_time_;
    this->end_time_ = now;
  });
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR
