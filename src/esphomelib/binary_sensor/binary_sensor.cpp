#include "esphomelib/defines.h"

#ifdef USE_BINARY_SENSOR

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace binary_sensor {

static const char *TAG = "binary_sensor";

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
  ESP_LOGD(TAG, "'%s': Sending state %s", this->get_name().c_str(), state ? "ON" : "OFF");
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
BinarySensor::BinarySensor()
  : BinarySensor("") {

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

MultiClickTrigger *BinarySensor::make_multi_click_trigger(const std::vector<MultiClickTriggerEvent> &timing) {
  return new MultiClickTrigger(this, timing);
}

MultiClickTrigger::MultiClickTrigger(BinarySensor *parent, const std::vector<MultiClickTriggerEvent> &timing)
    : parent_(parent), timing_(timing) {

}
void MultiClickTrigger::setup() {
  this->last_state_ = this->parent_->state;
  auto f = std::bind(&MultiClickTrigger::on_state_, this, std::placeholders::_1);
  this->parent_->add_on_state_callback(f);
}
float MultiClickTrigger::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
void MultiClickTrigger::on_state_(bool state) {
  // Handle duplicate events
  if (state == this->last_state_) {
    return;
  }
  this->last_state_ = state;

  // Cooldown: Do not immediately try matching after having invalid timing
  if (this->is_in_cooldown_) {
    return;
  }

  if (!this->at_index_.has_value()) {
    // Start matching
    MultiClickTriggerEvent evt = this->timing_[0];
    if (evt.state == state) {
      ESP_LOGV(TAG, "START min=%u max=%u", evt.min_length, evt.max_length);
      ESP_LOGV(TAG, "Multi Click: Starting multi click action!");
      this->at_index_ = 1;
      this->schedule_is_valid_(evt.min_length);
      this->schedule_is_not_valid_(evt.max_length);
    } else {
      ESP_LOGV(TAG, "Multi Click: action not started because first level does not match!");
    }

    return;
  }

  if (!this->is_valid_) {
    this->schedule_cooldown_();
    return;
  }

  if (*this->at_index_ == this->timing_.size()) {
    this->trigger_();
    return;
  }

  MultiClickTriggerEvent evt = this->timing_[*this->at_index_];

  if (evt.max_length != 4294967294UL) {
    ESP_LOGV(TAG, "A i=%u min=%u max=%u", *this->at_index_, evt.min_length, evt.max_length);
    this->schedule_is_valid_(evt.min_length);
    this->schedule_is_not_valid_(evt.max_length);
  } else if (*this->at_index_ + 1 != this->timing_.size()) {
    ESP_LOGV(TAG, "B i=%u min=%u", *this->at_index_, evt.min_length);
    this->cancel_timeout("is_not_valid");
    this->schedule_is_valid_(evt.min_length);
  } else {
    ESP_LOGV(TAG, "C i=%u min=%u", *this->at_index_, evt.min_length);
    this->is_valid_ = false;
    this->cancel_timeout("is_not_valid");
    this->set_timeout("trigger", evt.min_length, [this]() {
      this->trigger_();
    });
  }

  *this->at_index_ = *this->at_index_ + 1;
}
void MultiClickTrigger::set_invalid_cooldown(uint32_t invalid_cooldown) {
  this->invalid_cooldown_ = invalid_cooldown;
}
void MultiClickTrigger::schedule_cooldown_() {
  ESP_LOGV(TAG, "Multi Click: Invalid length of press, starting cooldown of %u ms...", this->invalid_cooldown_);
  this->is_in_cooldown_ = true;
  this->set_timeout("cooldown", this->invalid_cooldown_, [this]() {
    ESP_LOGV(TAG, "Multi Click: Cooldown ended, matching is now enabled again.");
    this->is_in_cooldown_ = false;
  });
  this->at_index_.reset();
  this->cancel_timeout("trigger");
  this->cancel_timeout("is_valid");
  this->cancel_timeout("is_not_valid");
}
void MultiClickTrigger::schedule_is_valid_(uint32_t min_length) {
  this->is_valid_ = false;
  this->set_timeout("is_valid", min_length, [this]() {
    ESP_LOGV(TAG, "Multi Click: You can now %s the button.", this->parent_->state ? "RELEASE" : "PRESS");
    this->is_valid_ = true;
  });
}
void MultiClickTrigger::schedule_is_not_valid_(uint32_t max_length) {
  this->set_timeout("is_not_valid", max_length, [this]() {
    ESP_LOGV(TAG, "Multi Click: You waited too long to %s.", this->parent_->state ? "RELEASE" : "PRESS");
    this->is_valid_ = false;
    this->schedule_cooldown_();
  });
}
void MultiClickTrigger::trigger_() {
  ESP_LOGV(TAG, "Multi Click: Hooray, multi click is valid. Triggering!");
  this->at_index_.reset();
  this->cancel_timeout("trigger");
  this->cancel_timeout("is_valid");
  this->cancel_timeout("is_not_valid");
  this->trigger();
}

} // namespace binary_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BINARY_SENSOR
